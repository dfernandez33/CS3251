import java.io.Serializable;

class ClientMessage implements Serializable {
    int messageLength;
    String data;

    ClientMessage(int length, String data) {
        this.messageLength = length;
        this.data = data;
    }
}
