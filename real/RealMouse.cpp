#include <tuple>
#include <common/core/Mouse.h>
#include "RealMouse.h"

RealMouse *RealMouse::instance = nullptr;

IRConverter::IRConverter() : ir_lookup{
    755, // .01
    648, // .02
    492, // .03
    409, // .04
    315, // .05
    268, // .06
    224, // .07
    192, // .08
    165, // .09
    147, // .10
    134, // .11
    115, // .12
    105, // .13
    93,  // .14
    86,  // .15
    75,  // .16
    68,  // .17
    58,  // .18
}, calibration_offset(0) {}

void IRConverter::calibrate(int avg_adc_value_on_center) {
  double actual_dist = (0.08 - smartmouse::kc::FRONT_SIDE_ANALOG_Y) / sin(smartmouse::kc::FRONT_ANALOG_ANGLE);
  int centered_idx = 4;
  double expected_distance = (avg_adc_value_on_center - ir_lookup[centered_idx]) * 0.01 /
      (ir_lookup[centered_idx] - ir_lookup[centered_idx - 1]) + (centered_idx + 1) * .01;
  calibration_offset = actual_dist - expected_distance;
}

double IRConverter::adcToMeters(int adc) {
  if (adc > 751) {
    return 0.01;
  } else if (adc <= 53) {
    return 0.18;
  } else {
    for (int i = 1; i < 18; i++) {
      if (adc >= ir_lookup[i]) {
        // linear map between i and i+1, .01 meters spacing
        double o = (adc - ir_lookup[i]) * 0.01 / (ir_lookup[i] - ir_lookup[i - 1]) + (i + 1) * .01 + calibration_offset;
        return o;
      }
    }
    return 0.18;
  }
}

RealMouse *RealMouse::inst() {
  if (instance == NULL) {
    instance = new RealMouse();
  }
  return instance;
}

RealMouse::RealMouse()
    : kinematic_controller(this),
      range_data({0.18, 0.18, 0.18, 0.18, 0.18}),
      left_encoder(LEFT_ENCODER_CS),
      right_encoder(RIGHT_ENCODER_CS) {}

SensorReading RealMouse::checkWalls() {
  SensorReading sr(row, col);

  sr.walls[static_cast<int>(dir)] = range_data.front < 0.17;
  sr.walls[static_cast<int>(left_of_dir(dir))] = range_data.gerald_left < 0.15;
  sr.walls[static_cast<int>(right_of_dir(dir))] = range_data.gerald_right < 0.15;
  sr.walls[static_cast<int>(opposite_direction(dir))] = false;

  return sr;
}

GlobalPose RealMouse::getGlobalPose() {
  return kinematic_controller.getGlobalPose();
}

LocalPose RealMouse::getLocalPose() {
  return kinematic_controller.getLocalPose();
}

std::pair<double, double> RealMouse::getWheelVelocities() {
  return kinematic_controller.getWheelVelocitiesCPS();
};

void RealMouse::run(double dt_s) {
  double abstract_left_force, abstract_right_force;
  left_angle_rad = tick_to_rad(left_encoder.getRotation());
  right_angle_rad = tick_to_rad(right_encoder.getRotation());

#ifdef PROFILE
  unsigned long t0 = micros();
#endif
  range_data.gerald_left = ir_converter.adcToMeters(analogRead(GERALD_LEFT_ANALOG_PIN));
  range_data.gerald_right = ir_converter.adcToMeters(analogRead(GERALD_RIGHT_ANALOG_PIN));
  range_data.front_left = ir_converter.adcToMeters(analogRead(FRONT_LEFT_ANALOG_PIN));
  range_data.back_left = ir_converter.adcToMeters(analogRead(BACK_LEFT_ANALOG_PIN));
  range_data.front_right = ir_converter.adcToMeters(analogRead(FRONT_RIGHT_ANALOG_PIN));
  range_data.back_right = ir_converter.adcToMeters(analogRead(BACK_RIGHT_ANALOG_PIN));
  range_data.front = ir_converter.adcToMeters(analogRead(FRONT_ANALOG_PIN));

#ifdef PROFILE
  Serial.print("Sensors, ");
  Serial.println(micros() - t0);
#endif

#ifdef PROFILE
  unsigned long t1 = micros();
#endif
  std::tie(abstract_left_force, abstract_right_force) = kinematic_controller.run(dt_s, left_angle_rad,
                                                                                 right_angle_rad, range_data);
#ifdef PROFILE
  Serial.print("KC, ");
  Serial.println(micros() - t1);
#endif

  // THIS IS SUPER IMPORTANT!
  // update row/col information
  row = kinematic_controller.row;
  col = kinematic_controller.col;

#ifdef PROFILE
  unsigned long t2 = micros();
#endif
  abstract_left_force = 140;
  abstract_right_force = 0;
  if (abstract_left_force < 0) {
    analogWrite(MOTOR_LEFT_A1, (int) -abstract_left_force);
    analogWrite(MOTOR_LEFT_A2, 0);
  } else {
    analogWrite(MOTOR_LEFT_A1, 0);
    analogWrite(MOTOR_LEFT_A2, (int) abstract_left_force);
  }

  if (abstract_right_force < 0) {
    analogWrite(MOTOR_RIGHT_B2, 0);
    analogWrite(MOTOR_RIGHT_B1, (int) -abstract_right_force);
  } else {
    analogWrite(MOTOR_RIGHT_B2, (int) abstract_right_force);
    analogWrite(MOTOR_RIGHT_B1, 0);
  }
#ifdef PROFILE
  Serial.print("Motors, ");
  Serial.println(micros() - t2);
#endif
}

void RealMouse::setup() {
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);
  pinMode(LED_6, OUTPUT);
  pinMode(LED_7, OUTPUT);
  pinMode(SYS_LED, OUTPUT);
  pinMode(MOTOR_LEFT_A1, OUTPUT);
  pinMode(MOTOR_LEFT_A2, OUTPUT);
  pinMode(MOTOR_RIGHT_B1, OUTPUT);
  pinMode(MOTOR_RIGHT_B2, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  left_encoder.init();
  right_encoder.init();
  // pull MOSI high to run encoders in 3 wire mode
  pinMode(MOSI, OUTPUT);
  digitalWrite(MOSI, HIGH);

//  analogWriteFrequency(MOTOR_LEFT_A1, 1831.055);
//  analogWriteFrequency(MOTOR_LEFT_A2, 1831.055);
//  analogWriteFrequency(MOTOR_RIGHT_B1, 1831.055);
//  analogWriteFrequency(MOTOR_RIGHT_B2, 1831.055);
  analogReadResolution(13);

  kinematic_controller.setAccelerationCpss(10);

  resetToStartPose();

  // Teensy does USB in software, so serial rate doesn't do anything
  Serial.begin(0);
  // But it does here because it's not the USB serial
  Serial1.begin(115200);
}

void RealMouse::resetToStartPose() {
  reset(); // resets row, col, and dir
  left_angle_rad = tick_to_rad(left_encoder.getRotation());
  right_angle_rad = tick_to_rad(right_encoder.getRotation());
  kinematic_controller.left_motor.reset_enc_rad(left_angle_rad);
  kinematic_controller.right_motor.reset_enc_rad(right_angle_rad);
  kinematic_controller.reset_col_to(0.09);
  kinematic_controller.reset_row_to(0.09);
  kinematic_controller.reset_yaw_to(0.0);
}

void RealMouse::setSpeedCps(double l_cps, double r_cps) {
  kinematic_controller.setSpeedCps(l_cps, r_cps);
}

double RealMouse::checkVoltage() {
  // 3.2v is max and 2.7v is min
  int a = analogRead(BATTERY_ANALOG_PIN);
  double voltage = a / std::pow(2, 13) * 3.3;

  if (voltage < 2.7) {
    print("VOLTAGE IS TOO LOW. CHARGE THE BATTERY!!!\r\n");
  } else if (voltage > 3.2) {
    print("VOLTAGE IS TOO HIGH. SHE'S GONNA BLOW!!!\r\n");
  }

  return voltage;
}
