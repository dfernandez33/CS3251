import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

// Class used to handle game logic for each client that
// connects to the server
public class ClientHandler extends Thread {

    private final DataInputStream dis;
    private final DataOutputStream dos;
    private final Socket socket;
    private final String gameWord;
    private String incorrectGuesses;
    private String wordState;

    ClientHandler(Socket s, DataInputStream dis, DataOutputStream dos, String gameWord) {
        this.socket = s;
        this.dis = dis;
        this.dos = dos;
        this.gameWord = gameWord;
        this.incorrectGuesses = "";
        StringBuilder builder = new StringBuilder();
        for (int i = 0; i < gameWord.length(); i++) {
            builder.append("-");
        }
        this.wordState = builder.toString();
        System.out.println(this.gameWord);
    }

    @Override
    public void run() {
        //game logic goes in here
        boolean gameOver = false;
        while (!gameOver) {
            try {
                if (this.dis.available() > 0) {
                    byte[] mssgBuf = new byte[this.dis.available()];
                    this.dis.read(mssgBuf);
                    ClientMessage message = (ClientMessage) Utils.convertFromBytes(mssgBuf);
                    ServerMessage gameMessage;
                    if (message.messageLength == 0) { // send initial game control packet
                        gameMessage = new ServerMessage(0, this.gameWord.length(),
                                this.incorrectGuesses.length(), this.wordState + this.incorrectGuesses);
                        this.dos.write(Utils.convertToBytes(gameMessage));
                    } else { // client game packet
                        String guess = message.data;
                        if (this.gameWord.contains(guess)) {
                            updateWordState(guess);
                            gameMessage = new ServerMessage(0, this.gameWord.length(),
                                    this.incorrectGuesses.length(), this.wordState + this.incorrectGuesses);
                            this.dos.write(Utils.convertToBytes(gameMessage));
                            if (this.wordState.equals(gameWord)) {
                                gameMessage = new ServerMessage(8,"You Win!");
                                this.dos.write(Utils.convertToBytes(gameMessage));
                                gameOver = true;
                            }
                        } else {
                            this.incorrectGuesses += guess;
                            gameMessage = new ServerMessage(0, this.gameWord.length(),
                                    this.incorrectGuesses.length(), this.wordState + this.incorrectGuesses);
                            this.dos.write(Utils.convertToBytes(gameMessage));
                            if (this.incorrectGuesses.length() >= 6) {
                                gameMessage = new ServerMessage(9,"You Lose!");
                                this.dos.write(Utils.convertToBytes(gameMessage));
                                gameOver = true;
                            }
                        }

                    }
                }
            } catch(IOException | ClassNotFoundException e) {
                e.printStackTrace();
            }
        }



        // close all the resources
        try {
            this.dis.close();
            this.dos.close();
            // reduce the number of connections on the server by one using thread save method
            server.updateNumConnections(-1);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void updateWordState(String guess) {
        List<Integer> indecies = new ArrayList<>();
        int index = this.gameWord.indexOf(guess);
        while (index >= 0) {
            indecies.add(index);
            index = this.gameWord.indexOf(guess, index + 1);
        }

        StringBuilder wordStateBuilder = new StringBuilder(this.wordState);
        for (Integer currIndex : indecies) {
            wordStateBuilder.setCharAt(currIndex, guess.charAt(0));
        }

        this.wordState = wordStateBuilder.toString();
    }
}
