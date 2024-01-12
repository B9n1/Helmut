import { Component, OnDestroy, OnInit } from '@angular/core';
import { ChangeDetectorRef } from '@angular/core';
import { SseService } from '../services/sse.service';

export interface SensorData {
  max_x: number;
  max_y: number;
  max_z: number;
}

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit, OnDestroy {
  CRASH_THRESHOLD: number = 100;
  sensorData: SensorData = {} as SensorData;
  alert: boolean = false;
  maxValue: number = 0;
  maxValueCrash: number = 30;

  constructor(private sseService: SseService, private cdr: ChangeDetectorRef) {}

  ngOnInit(): void {
    this.subscribeToSensorData();
  }

  getMaxValueRoundedTo2Digits(v1: number, v2: number, v3: number) {
    let tempMaxValue = Math.round(v1 * 100) / 100;
    if ((Math.round(v2 * 100) / 100) > tempMaxValue) {
      tempMaxValue = v2;
    }
    if ((Math.round(v3 * 100) / 100) > tempMaxValue) {
      tempMaxValue = v3;
    }
    // console.log("Max value between " + v1 + ", " + v2 + ", " + v3 + " is " + (Math.round(tempMaxValue * 100) / 100));
    return (Math.round(tempMaxValue * 100) / 100);
  }

  private subscribeToSensorData() {
    this.sseService.getSensorData().subscribe(
      (data: SensorData) => {
        // Handle the real-time sensor data
        this.sensorData = data;
        if (this.sensorData.max_x > this.CRASH_THRESHOLD || this.sensorData.max_y > this.CRASH_THRESHOLD || this.sensorData.max_z > this.CRASH_THRESHOLD) {
          this.alert = true;
          if (this.maxValueCrash < this.getMaxValueRoundedTo2Digits(this.sensorData.max_x, this.sensorData.max_y, this.sensorData.max_z)) {
            this.maxValueCrash = this.getMaxValueRoundedTo2Digits(this.sensorData.max_x, this.sensorData.max_y, this.sensorData.max_z);
          }
        }
        if (this.maxValue < this.getMaxValueRoundedTo2Digits(this.sensorData.max_x, this.sensorData.max_y, this.sensorData.max_z) && this.alert == false) {
          this.maxValue = this.getMaxValueRoundedTo2Digits(this.sensorData.max_x, this.sensorData.max_y, this.sensorData.max_z);
        }
        this.cdr.detectChanges();
      },
      (error) => {
        console.error('Error in SSE subscription:', error);
      }
    );
  }

  ngOnDestroy(): void {
    // Close the SSE connection when the component is destroyed
    this.sseService.closeConnection();
  }
}
