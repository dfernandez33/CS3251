import java.io.Serializable;

class ServerMessage implements Serializable {
    int mssgFlag;
    int wordLength;
    int numIncorrect;
    String data;

    ServerMessage(int flag, int length, int incorrect, String data) { // constructor for game control packet
        this.mssgFlag = flag;
        this.wordLength = length;
        this.numIncorrect = incorrect;
        this.data = data;
    }

    ServerMessage(int flag, String data) { // constructor for game message packet
        this.mssgFlag = flag;
        this.data = data;
    }
}
