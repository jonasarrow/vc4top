/* 
    vc4top show utilization of Broadcom's VC4 GPU.
    Copyright (C) 2017 Jonas Pfeil

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include "vc4_defines.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <memory.h>
#include <gtk/gtk.h>
#include "gtk_window.xml.h"


using namespace std;
using namespace std::chrono;
//Constants, may be incorporated in some user setting (if you program it)
auto sampling_interval = 100ms;
auto update_interval = 500ms;



volatile uint32_t* base=0;
struct counter_status {
	int idle_count;
	long long counts[16];
	double timediff;
	const counter_status& operator-=(const counter_status&rhs) {
		idle_count -= rhs.idle_count;
		timediff -= rhs.timediff;
		for (int i = 0; i < 16; ++i)
			counts[i] -= rhs.counts[i];
		return *this;
	}
	const counter_status operator-(const counter_status&rhs) {
		return counter_status(*this) -= rhs;
	}
};
struct window{
	GtkWidget*mainwindow,
		*frequency,
		*idle,
		*stall,
		*busy,
		*vertex,
		*fragment,
		*plot;
	std::mutex lock;
	bool running = true;
	std::vector<counter_status>plotinfo;
};
void enable_counter(int id)
{
    uint32_t oldval=VC4_READ(base,V3D_PCTRE);
    VC4_WRITE(base, V3D_PCTRE,oldval|(1<<id));
}
void disable_counter(int id)
{
    VC4_WRITE(base, V3D_PCTRE,VC4_READ(base,V3D_PCTRE)&~(1<<id));
}

void set_counter_source(int counter,int source)
{
    VC4_WRITE(base, V3D_PCTRS0+8*counter,source);
}

uint32_t get_counter(int counter)
{
    return VC4_READ(base, V3D_PCTR0+8*counter);
}
void clear_counter(int counter)
{
    VC4_WRITE(base, V3D_PCTRC, 1<<counter);
}
// Detected by observation, if the GPU is gated, deadbeef is returned
#define POWEROFF_VALUE 0xdeadbeef

#define ACTIVE_COUNTERS 7
void start_counters()
{
	//Activate counter mechanism
    enable_counter(31);
    for(int i=0;i<ACTIVE_COUNTERS;++i){
        enable_counter(i);
        clear_counter(i);
        set_counter_source(i, 13+i);
    }
}

void counter_loop(window*w) 
{
	auto last_sampling_time = high_resolution_clock::now();
	auto last_update_time = last_sampling_time;
	counter_status current = {}, last_update = {};
	bool idling = true;
	start_counters();
	while (w->running) {
		::this_thread::sleep_until(last_sampling_time + sampling_interval);
		auto now = high_resolution_clock::now();
		last_sampling_time = now;
		//Update all counters
		if (get_counter(0) == POWEROFF_VALUE) {
			current.idle_count++;
			idling = true;
		}
		else {
			if (idling) {
				idling = false;
				start_counters();
			}
			for (int i = 0; i < ACTIVE_COUNTERS; ++i) {
				uint32_t value = get_counter(i);
				clear_counter(i);
				current.counts[i] += value;
			}
		}
		if (now > last_update_time + update_interval) {
			counter_status diff = current - last_update;
			diff.timediff=duration<double>(now - last_update_time).count();
			last_update_time = now;
			last_update = current;
			std::lock_guard<std::mutex>l(w->lock);
			w->plotinfo.push_back(diff);
			gtk_widget_queue_draw(w->plot);
		}
		for(int i=0;i<16;++i)
			clear_counter(i);
	}
}
G_MODULE_EXPORT extern "C" gboolean plot_draw_cb(GtkWidget*area, cairo_t*cr, gpointer ptr)
{
	window*w = (window*)ptr;
	std::lock_guard<std::mutex>l(w->lock);
	if (!w->plotinfo.size())
		return FALSE;
	guint width = gtk_widget_get_allocated_width(area);
	guint height = gtk_widget_get_allocated_height(area);

	//Only display the last 100 records
	if (w->plotinfo.size() > 100)
		w->plotinfo.erase(w->plotinfo.begin(), w->plotinfo.begin() + (w->plotinfo.size() - 100));

	//Draw the graphical stats
	int numelement = w->plotinfo.size();
	cairo_set_source_rgb(cr, 0, 1, 0);
	cairo_move_to(cr, 0, height);
	for (int i = 0; i < numelement; ++i) {
		auto diff = w->plotinfo[i];
		long long total_clock = diff.counts[0] + diff.counts[1] + diff.counts[2] + diff.counts[3] + diff.counts[4] + diff.counts[5] + diff.counts[6];
		double pos = diff.counts[1] *1.0/ total_clock;
		cairo_line_to(cr, width*i / (max(1,numelement-1)), height - pos*height);
	}
	cairo_line_to(cr, width, height);
	cairo_close_path(cr);
	cairo_fill(cr);
	
	//Update the text stats
	counter_status diff = *(w->plotinfo.rbegin());
	double timediff = diff.timediff;
	double total_clock = diff.counts[0] + diff.counts[1] + diff.counts[2] + diff.counts[3] + diff.counts[4] + diff.counts[5] + diff.counts[6];
	printf("t = %f: ", timediff);
	printf("%016llx, %016llx, %016llx, %016llx, %016llx, %016llx, %016llx\n", diff.counts[0], diff.counts[1], diff.counts[2], diff.counts[3], diff.counts[4], diff.counts[5], diff.counts[6]);
	gtk_label_set_text(GTK_LABEL(w->frequency), (
		std::to_string(total_clock / 12e6 / timediff) + " MHz").c_str());

	gtk_label_set_text(GTK_LABEL(w->idle), (std::to_string(
		diff.idle_count*1e2*sampling_interval.count() / update_interval.count())
		+ " %").c_str());

	gtk_label_set_text(GTK_LABEL(w->stall), (std::to_string(
		(diff.counts[4] + diff.counts[5] + diff.counts[6])/total_clock*1.e2)
		+ " %").c_str());

	gtk_label_set_text(GTK_LABEL(w->busy), (std::to_string(
		(diff.counts[1] + diff.counts[2] + diff.counts[3])/total_clock*1.e2)
		+ " %").c_str());

	gtk_label_set_text(GTK_LABEL(w->vertex), (std::to_string(
		(diff.counts[1])/total_clock*1.e2)
		+ " %").c_str());
	gtk_label_set_text(GTK_LABEL(w->fragment), (std::to_string(
		(diff.counts[2])/total_clock*1.e2)
		+ " %").c_str());
	return FALSE;
}
int main(int argc, char *argv[])
{
    int fd=0;
    if((fd=open("/dev/mem", O_RDWR|O_SYNC))<0){
        perror("Open of /dev/mem failed, are you root");
        return -1;
    }
    if((base=(unsigned*)mmap(
            NULL,
            VC4_BLOCK_SIZE,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd,
            VC4_BASE)
            )==MAP_FAILED){
        perror("Mapping register file failed");
        return -2;
    }
    close(fd);
    
	window w = {};
    gtk_init(&argc,&argv);
	GtkBuilder*build=gtk_builder_new_from_string((const gchar*)gtk_window_xml,sizeof(gtk_window_xml));
    w.mainwindow = GTK_WIDGET(gtk_builder_get_object(build, "mainwindow"));
	w.frequency = GTK_WIDGET(gtk_builder_get_object(build, "frequency"));
	w.idle = GTK_WIDGET(gtk_builder_get_object(build, "idle"));
	w.stall = GTK_WIDGET(gtk_builder_get_object(build, "stall"));
	w.busy = GTK_WIDGET(gtk_builder_get_object(build, "busy"));
	w.vertex = GTK_WIDGET(gtk_builder_get_object(build, "vertex"));
	w.fragment = GTK_WIDGET(gtk_builder_get_object(build, "fragment"));
	w.plot = GTK_WIDGET(gtk_builder_get_object(build, "plot"));
	gtk_builder_connect_signals(build, (gpointer)&w);
    gtk_widget_show_all(w.mainwindow);
	auto th = std::thread(counter_loop, &w);
    gtk_main();
	w.running=false;
	th.join();
    g_object_unref(G_OBJECT(build));
}
