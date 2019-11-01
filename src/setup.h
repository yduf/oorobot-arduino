void setup() {
  Serial.begin(9600);
  //loadParams();
  setupButtons();
  Serial.println(F("Setup"));

  lcd.setup();

  // stepper motors init
  stepper1.setMaxSpeed(900);
  stepper1.move(1);

  stepper2.setMaxSpeed(900);
  stepper2.move(-1);

  moveServo(5);

  // Bluetooth module init
#ifdef HAVE_BLUETOOTH
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  BTSerie.begin(9600);
#endif 
}
