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
volatile unsigned* base=0;
struct{
	GtkWidget*mainwindow,
		*frequency,
		*idle,
		*stall,
		*busy,
		*vertex,
		*fragment,
		*plot;
	std::mutex lock;
}window;
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

void start_counters()
{
	//Activate counter mechanism
    enable_counter(31);
    for(int i=0;i<7;++i){
        enable_counter(i);
        clear_counter(i);
        set_counter_source(i, 13+i);
    }
}
struct counter_status{
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
bool running = true;

std::vector<counter_status>plotinfo;
void counter_loop() {
	using namespace std::chrono;
	auto sampling_interval = 10ms;
	auto update_interval = 100ms;
	auto last_sampling_time = high_resolution_clock::now();
	auto last_update_time = last_sampling_time;
	counter_status current = {}, last_update = {};
	uint32_t last_count[16] = {};
	bool idling = true;
	start_counters();
	while (running) {
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
				memset(last_count, 0, sizeof(last_count));
			}
			for (int i = 0; i < 16; ++i) {
				uint32_t value = get_counter(i);
				current.counts[i] += value - last_count[i];
				last_count[i] = value;
			}
		}
		if (now > last_update_time + update_interval) {
			counter_status diff = current - last_update;
			double timediff = duration<double>(now - last_update_time).count();
			printf("t = %f: ", timediff);
			diff.timediff = timediff;
			double total_clock = 1.0*diff.counts[0] + diff.counts[1] + diff.counts[2]+ diff.counts[3] + diff.counts[4] + diff.counts[5] + diff.counts[6];
			//Update the window
			//Maybe put all into a marshalled callback
			//gdk_threads_enter();
			printf("%08x, %08x, %08x, %08x, %08x, %08x, %08x\n", diff.counts[0], diff.counts[1], diff.counts[2], diff.counts[3], diff.counts[4], diff.counts[5], diff.counts[6]);
			gtk_label_set_text(GTK_LABEL(window.frequency), (
				std::to_string(total_clock / 12e6 / timediff) + " MHz").c_str());

			gtk_label_set_text(GTK_LABEL(window.idle), (std::to_string(
				(
					diff.idle_count
					)*1.e2*(sampling_interval.count() / update_interval.count()))
				+ " %").c_str());

			gtk_label_set_text(GTK_LABEL(window.stall), (std::to_string(
				(
				(diff.counts[4] + diff.counts[5] + diff.counts[6])
					/
					(1.0*total_clock)
					)*1.e2)
				+ " %").c_str());

			gtk_label_set_text(GTK_LABEL(window.busy), (std::to_string(
				(
				(diff.counts[1] + diff.counts[2] + diff.counts[3])
					/
					(1.0*total_clock)
					)*1.e2)
				+ " %").c_str());

			gtk_label_set_text(GTK_LABEL(window.vertex), (std::to_string(
				(
				(diff.counts[1])
					/
					(1.0*total_clock)
					)*1.e2)
				+ " %").c_str());
			gtk_label_set_text(GTK_LABEL(window.fragment), (std::to_string(
				(
				(diff.counts[2])
					/
					(1.0*total_clock)
					)*1.e2)
				+ " %").c_str());
			{
				std::lock_guard<std::mutex>l(window.lock);
				plotinfo.push_back(diff);
				gtk_widget_queue_draw(window.plot);
			}
			//gdk_threads_leave();
			last_update_time = now;
			last_update = current;
		}
		for(int i=0;i<16;++i)
			clear_counter(i);
	}
}
G_MODULE_EXPORT extern "C" void plot_draw_cb(GtkWidget*area, cairo_t*cr, gpointer ptr)
{
	std::lock_guard<std::mutex>l(window.lock);
	guint width = gtk_widget_get_allocated_width(area);
	guint height = gtk_widget_get_allocated_height(area);

	while (plotinfo.size() > 100)
		plotinfo.erase(plotinfo.begin(), plotinfo.begin() + (plotinfo.size() - 100));

	int numelement = plotinfo.size();
	cairo_set_source_rgb(cr, 0, 1, 0);
	cairo_move_to(cr, 0, height);
	for (int i = 0; i < numelement; ++i) {
		auto diff = plotinfo[i];
		long long total_clock = diff.counts[0] + diff.counts[1] + diff.counts[2] + diff.counts[3] + diff.counts[4] + diff.counts[5] + diff.counts[6];
		double pos = diff.counts[1] *1.0/ total_clock;
		cairo_line_to(cr, width*i / (max(1,numelement-1)), height - pos*height);
	}
	cairo_line_to(cr, width, height);
	cairo_close_path(cr);
	cairo_fill(cr);
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
    
    gtk_init(&argc,&argv);
	GtkBuilder*build=gtk_builder_new_from_string((const gchar*)gtk_window_xml,sizeof(gtk_window_xml));
    window.mainwindow = GTK_WIDGET(gtk_builder_get_object(build, "mainwindow"));
	window.frequency = GTK_WIDGET(gtk_builder_get_object(build, "frequency"));
	window.idle = GTK_WIDGET(gtk_builder_get_object(build, "idle"));
	window.stall = GTK_WIDGET(gtk_builder_get_object(build, "stall"));
	window.busy = GTK_WIDGET(gtk_builder_get_object(build, "busy"));
	window.vertex = GTK_WIDGET(gtk_builder_get_object(build, "vertex"));
	window.fragment = GTK_WIDGET(gtk_builder_get_object(build, "fragment"));
	window.plot = GTK_WIDGET(gtk_builder_get_object(build, "plot"));
	gtk_builder_connect_signals(build, nullptr);
    gtk_widget_show_all(window.mainwindow);
	auto th=std::thread(counter_loop);
    gtk_main();
	running=false;
	th.join();
    g_object_unref(G_OBJECT(build));
    uint32_t old_vals[7]={};
    auto old_ts=chrono::high_resolution_clock::now();
    bool started=false;
    for(;;this_thread::sleep_for(0.5s)){
        if(get_counter(0)==POWEROFF_VALUE){
            cout<<"Busy: idle\n";
            started=false;
            continue;
        }
        if(!started){
            start_counters();
            memset(old_vals,0,sizeof(old_vals));
            started=true;
        }
        uint32_t new_vals[7]={};
        for(int i=0;i<7;++i){
            new_vals[i]=get_counter(i);
        }
        uint32_t idle_clocks=new_vals[0]-old_vals[0];
        uint32_t busy_clocks=0;
        uint32_t stall_clocks=0;
        for(int i=1;i<7;++i)
            busy_clocks+=new_vals[i]-old_vals[i];
        for(int i=4;i<7;++i)
            stall_clocks+=new_vals[i]-old_vals[i];


        auto end=chrono::high_resolution_clock::now();
        double dur=chrono::duration<double>(end-old_ts).count();


        cout<<"Busy: "<<((double)busy_clocks)*1e2/((double)busy_clocks+idle_clocks)<<"\n";
        cout<<"Stall: "<<(double)stall_clocks*1e2/busy_clocks<<" %\n";
        cout<<"Frequency: "<<((idle_clocks+busy_clocks)/dur/12e6)<<" MHz\n";

        memcpy(old_vals,new_vals,sizeof(old_vals));
        old_ts=end;
    }
    return 0;
}
