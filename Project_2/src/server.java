import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

// server.java
// Handles up to 3 clients
// each client plays their own game
class server
{
    private static List<String> wordBank = new ArrayList<>(Arrays.asList("amount", "voice", "oven", "snake",
            "umbrella", "volcano", "request", "month", "slope", "bite", "form", "trucks",
            "man", "building", "toys"));

    private static int numConnections = 0;

    public static void main(String args[]) throws IOException {
        String ipAddrress = args[0]; // not used for now
        int port = Integer.valueOf(args[1]);

        ServerSocket serverSocket = new ServerSocket(port);

        while (true) {
            Socket clientSocket = null;
            try {
                clientSocket = serverSocket.accept();
                if (numConnections >= 3) {
                    serverOverload(clientSocket);
                    continue;
                } else {
                    updateNumConnections(1);
                }
                // add logic to handle choosing word from bank
                String gameword = wordBank.get((int)(Math.random() * wordBank.size()));
                DataInputStream dis = new DataInputStream(clientSocket.getInputStream());
                DataOutputStream dos = new DataOutputStream(clientSocket.getOutputStream());

                Thread clientThread = new ClientHandler(clientSocket, dis, dos, gameword);
                clientThread.start();
            } catch (Exception e) {
                clientSocket.close();
                e.printStackTrace();
            }
        }
    }

    private static void serverOverload(Socket clientSocket) throws IOException {
        // send server-overloaded message
        ServerMessage overloadMessage = new ServerMessage(17,"server-overloaded");
        clientSocket.getOutputStream().write(Utils.convertToBytes(overloadMessage));
    }

    static synchronized void updateNumConnections(int amount) {
        numConnections += amount;
    }
}