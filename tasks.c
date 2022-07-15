#include <stdio.h>
#include "config.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "tasks.h"
#include <stdlib.h>

#define TASK3 1

TaskHandle_t hand[3] = {NULL};
TaskHandle_t master_hand[3] = {NULL};
int deadlines[3] = {0};

int getTemperature()
{
	return 10+rand()*80/RAND_MAX;
}

int getPressure()
{
	return 2+rand()*8/RAND_MAX;
}

int getHeight()
{
	return 100+rand()*900/RAND_MAX;
}

void temp_read(void* p)
{
	printf("Temperature reading: %d\n", getTemperature());
	fflush(stdout);
}
void pressure_read(void* p)
{
	printf("Pressure reading: %d\n", getPressure());
	fflush(stdout);
}
void height_read(void* p)
{
	printf("Height reading: %d\n", getHeight());
	fflush(stdout);
}

void sort()
{
	for(int i=0;i<3;i++)
	{
		for(int j=i+1;j<3;j++)
		{
			if(deadlines[i]>deadlines[j])
			{
				int temp = uxTaskPriorityGet(hand[i]);
				vTaskPrioritySet(hand[i], uxTaskPriorityGet(hand[j]));
				vTaskPrioritySet(hand[j], temp);
			}
			else if(deadlines[i]==deadlines[j])
			{
				if(uxTaskPriorityGet(hand[i]) < uxTaskPriorityGet(hand[j]))
				{
					int temp = uxTaskPriorityGet(hand[i]);
					vTaskPrioritySet(hand[i], uxTaskPriorityGet(hand[j]));
					vTaskPrioritySet(hand[j], temp);
				}
			}

		}
	}
}
void create_t(void* p)
{
	vTaskDelay(PERIOD_T/portTICK_PERIOD_MS);
	xTaskCreate(temp_read, "Job Temperature", 200, (void*)0, tskIDLE_PRIORITY + PRIORITY_T, &hand[0]);
	deadlines[0] += PERIOD_T;
	if (TASK3 == 0)
		return;
	sort();
}

void create_p(void *p)
{
	vTaskDelay(PERIOD_P/portTICK_PERIOD_MS);
	xTaskCreate(pressure_read, "Job Pressure", 200, (void*)0, tskIDLE_PRIORITY + PRIORITY_P, &hand[1]);
	deadlines[1] += PERIOD_P;
	if (TASK3 == 0)
		return;
	sort();
}

void create_h(void* p)
{
	vTaskDelay(PERIOD_H/portTICK_PERIOD_MS);
	xTaskCreate(height_read, "Job Height", 200, (void*)0, tskIDLE_PRIORITY + PRIORITY_H, &hand[2]);
	deadlines[2] += PERIOD_H;
	if (TASK3 == 0)
		return;
	sort();
}

void vScheduleEDF() //is called from main.c
{
	xTaskCreate(create_t, "master_t", 200, (void*)0, configMAX_PRIORITIES-1, &master_hand[0]);
	xTaskCreate(create_p, "master_p", 200, (void*)0, configMAX_PRIORITIES-1, &master_hand[1]);
	xTaskCreate(create_h, "master_h", 200, (void*)0, configMAX_PRIORITIES-1, &master_hand[2]);
	vTaskStartScheduler();

}

