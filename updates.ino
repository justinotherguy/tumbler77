void accelerate() {
  Serial.println ("accelerate");
  if (stepper.getDirection()) {
    Serial.println("reverse");
  } else {
    Serial.println("forward");
  }
  for (int i=steps; i<RPM; i++) {
    stepper.begin(i, MICROSTEPS);
    if (forward) {
      stepper.rotate(steps); 
    } else {
      stepper.rotate(-steps); 
    }
  }
}

void decelerate() {
  Serial.println ("decelerate");
  for (int i=RPM; i>steps; i--) {
    stepper.begin(i, MICROSTEPS);
    if (forward) {
      stepper.rotate(steps); 
    } else {
      stepper.rotate(-steps); 
    }
  }
}

void change_dir() {
  if (forward) { // currently running forward, change direction to reverse
    Serial.println("forward -> reverse");    
    client.publish(mqtt_debug_topic, "forward -> reverse");
    decelerate();
    forward=false;
    accelerate();
  } else {       // currently running in reverse, change direction to forward
    Serial.println("reverse -> forward");    
    client.publish(mqtt_debug_topic, "reverse -> forward");
    decelerate();
    forward=true;
    accelerate();
  }
  Serial.print("time remaing [hh:mm:ss]: ");
  Serial.printf("%01d:%02d:%02d", remaining_hours, remaining_minutes, remaining_seconds);  
  Serial.println("");

  itoa(remaining_minutes, remaining_minutes_char, 10);
  itoa(remaining_hours, remaining_hours_char, 10);

  strcpy(remaining_time, remaining_hours_char);
  strcat(remaining_time, ":");
  strcat(remaining_time, remaining_minutes_char);
  client.publish(mqtt_pub_topic, remaining_time);

}

void update_time() {
  if (remaining_seconds > 0 ) {
    remaining_seconds--;
  } else {
    remaining_seconds=59;
    if (remaining_minutes > 0 ) {
      remaining_minutes--;
      itoa(remaining_seconds, remaining_seconds_char, 10);
      itoa(remaining_minutes, remaining_minutes_char, 10);
      itoa(remaining_hours, remaining_hours_char, 10);

      strcpy(remaining_time, remaining_hours_char);
      strcat(remaining_time, ":");
      strcat(remaining_time, remaining_minutes_char);
      client.publish(mqtt_pub_topic, remaining_time);
      Serial.println(remaining_time);
    } else {
      remaining_minutes=59;
      if (remaining_hours > 0 ) {
        remaining_hours--;      
      }
    }
  }
  #ifdef debug
    itoa(remaining_seconds, remaining_seconds_char, 10);
    itoa(remaining_minutes, remaining_minutes_char, 10);
    itoa(remaining_hours, remaining_hours_char, 10);

    strcpy(remaining_time, remaining_hours_char);
    strcat(remaining_time, ":");
    strcat(remaining_time, remaining_minutes_char);
//    strcat(remaining_time, ":");
//    strcat(remaining_time, remaining_seconds_char);
    client.publish(mqtt_pub_topic, remaining_time);
    Serial.println(remaining_time);
  #endif
}
