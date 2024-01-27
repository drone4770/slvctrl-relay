#include <SerialCommands.h>

const char* DEVICE_TYPE = "relay";
const int FM_VERSION = 10000; // 1.00.00
const int PROTOCOL_VERSION = 10000; // 1.00.00

const int RELAY_PIN = 2;
int relayState;

char serial_command_buffer[32];
SerialCommands serialCommands(&Serial, serial_command_buffer, sizeof(serial_command_buffer), "\n", " ");

void commandIntroduce(SerialCommands* sender);
void commandGetState(SerialCommands* sender);
void commandSetState(SerialCommands* sender);

void setup() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);
    relayState = 0;

    Serial.begin(9600);

    // Add commands
    serialCommands.SetDefaultHandler(commandUnrecognized);
    serialCommands.AddCommand(new SerialCommand("introduce", commandIntroduce));
    serialCommands.AddCommand(new SerialCommand("status", commandStatus));
    serialCommands.AddCommand(new SerialCommand("attributes", commandAttributes));
    serialCommands.AddCommand(new SerialCommand("get-state", commandGetState));
    serialCommands.AddCommand(new SerialCommand("set-state", commandSetState));

    serialCommands.GetSerial()->write(0x07);
}

void loop() {
    serialCommands.ReadSerial();
}

void commandIntroduce(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "introduce;%s,%d,%d\n", DEVICE_TYPE, FM_VERSION, PROTOCOL_VERSION);
}

void commandStatus(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "status;state:%d\n", relayState);
}

void commandAttributes(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "attributes;state:rw[bool]\n");
}

void commandSetState(SerialCommands* sender) {
    char* stateStr = sender->Next();

    if (stateStr == NULL) {
        sender->GetSerial()->println("set-state;;status:failed,reason:state_param_missing\n");
        return;
    }
  
    int state = atoi(stateStr);
  
    setState(state);

    serial_printf(sender->GetSerial(), "set-state;%d;status:successful\n", getState());
}

void commandGetState(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "get-state;%d\n", getState());
}

void commandUnrecognized(SerialCommands* sender, const char* cmd)
{
    serial_printf(sender->GetSerial(), "Unrecognized command [%s]\n", cmd);
}

void setState(int state) {
    if (state == 0) {
      digitalWrite(RELAY_PIN, HIGH);
      relayState = 0;
    } else {
      digitalWrite(RELAY_PIN, LOW);
      relayState = 1;
    }
}

int getState() {
    return relayState == 0 ? 0 : 1;
}
