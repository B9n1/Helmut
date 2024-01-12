import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root',
})
export class SseService {
  private eventSource: EventSource | undefined = undefined;
  private sseUrl = 'http://134.108.246.156:3002/sse';

  constructor() {}

  getSensorData(): Observable<any> {
    return new Observable<any>((observer) => {
      this.eventSource = new EventSource(this.sseUrl);

      this.eventSource.onmessage = (event) => {
        const eventData = JSON.parse(event.data);
        observer.next(eventData);
      };

      this.eventSource.onerror = (error) => {
        console.error('SSE Error:', error);
        observer.error(error);
      };
    });
  }

  closeConnection() {
    if (this.eventSource) {
      this.eventSource.close();
    }
  }
}
