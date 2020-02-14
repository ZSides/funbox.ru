typedef struct {
    bool FIN, SYN, RST, PSH, ACK, URG;
} tcpFlag;

void fillflag(tcpFlag *tcpflag, char byte_to_parse) {
    tcpflag->FIN = (byte_to_parse & 0b1); // 1
    tcpflag->SYN = (byte_to_parse & 0b10); // 2
    tcpflag->RST = (byte_to_parse & 0b100); // 4
    tcpflag->PSH = (byte_to_parse & 0b1000); // 8
    tcpflag->ACK = (byte_to_parse & 0b10000); // 16
    tcpflag->URG = (byte_to_parse & 0b100000); // 32
}