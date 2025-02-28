//Compile with arudino
//使用arudino编译
void idf_setup();
void idf_loop();




void setup() {
    pinMode(46, OUTPUT);
    digitalWrite(46,0);
    if (psramInit()) {
        Serial.println("\nThe PSRAM is correctly initialized");
    } else {
        Serial.println("\nPSRAM does not work");
    }

    idf_setup();
}

void loop() {
    idf_loop();
}
