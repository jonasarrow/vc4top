/*  This program is free software: you can redistribute it and/or modify
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
#include "vc4_defines.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <memory.h>
#include <chrono>
#include <thread>
using namespace std;
volatile unsigned* base=0;

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
    enable_counter(31);
    for(int i=0;i<7;++i){
        enable_counter(i);
        clear_counter(i);
        set_counter_source(i, 13+i);
    }
}
int main()
{
    int fd=0;
    if((fd=open("/dev/mem", O_RDWR|O_SYNC))<0){
        perror("Open of /dev/mem failed");
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
