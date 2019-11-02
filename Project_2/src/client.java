import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.util.HashSet;
import java.util.Scanner;
import java.util.Set;

// client.java
class client {
    public static void main(String args[]) throws IOException, ClassNotFoundException {
        InetAddress ipAddress = InetAddress.getByName(args[0]);
        int port = Integer.valueOf(args[1]);
        // set up scanner to read user input
        Scanner scanner = new Scanner(System.in);

        // ask user if she/he is ready to start game
        System.out.print("Ready to start game? (y/n): ");
        boolean start = false;
        Socket gameServerSocket = null;
        while (!start) {
            String resp = scanner.next();
            switch (resp) {
                case "y":
                    gameServerSocket = new Socket(ipAddress, port);
                    start = true;
                    System.out.println(); // print new line for aesthetic purposes
                    break;
                case "n":
                    return;
                default:
                    System.out.println("Please enter either y/n to continue.");
                    System.out.print("Ready to start game? (y/n): ");
            }
        }

        ClientMessage gameStartMessage = new ClientMessage(0, "");
        // obtaining input and out streams
        DataInputStream dis = new DataInputStream(gameServerSocket.getInputStream());
        DataOutputStream dos = new DataOutputStream(gameServerSocket.getOutputStream());
        // send game start packet
        dos.write(Utils.convertToBytes(gameStartMessage));


        // communicate with server until game over
        Set<String> guesses = new HashSet<>();
        boolean gameOver = false;
        while (!gameOver) {
            if (dis.available() > 0) {// if there are bytes to be read
                byte[] mssgBuf = new byte[dis.available()];
                dis.read(mssgBuf); // read server byte stream into buffer
                ServerMessage serverMessage = (ServerMessage) Utils.convertFromBytes(mssgBuf); // serialize byte[] into object
                if (serverMessage.mssgFlag == 0) { // game control packet received
                    // build the state of the word for printing
                    StringBuilder wordBuilder = new StringBuilder();
                    for (int i = 0; i < serverMessage.wordLength; i++) {
                        wordBuilder.append(serverMessage.data.charAt(i)).append(" ");
                    }
                    String word = wordBuilder.toString();

                    // build the string representing incorrect guesses
                    StringBuilder guessBuilder = new StringBuilder();
                    for (int i = serverMessage.wordLength; i < serverMessage.data.length(); i ++) {
                        guessBuilder.append(serverMessage.data.charAt(i)).append(" ");
                    }
                    String incorrectGuesses = guessBuilder.toString();

                    // display current game state
                    System.out.println(word);
                    System.out.println("Incorrect Guesses: " + incorrectGuesses);
                    System.out.println();

                    if (!word.contains("-") || incorrectGuesses.length() == 12) {
                        // this means game is over and the player either lost or won, so receive next message
                        continue;
                    }

                    // prompt user for new guess
                    System.out.print("Letter to guess: ");

                    // read user input and make sure it's valid
                    boolean validInput = false;
                    String guess = "";
                    while (!validInput) {
                        if (scanner.hasNextInt()) {
                            System.out.println("Error! Please guess one letter.");
                            System.out.println();
                            System.out.print("Letter to guess: ");
                            scanner.next();
                        } else {
                            guess = scanner.next().toLowerCase();
                            if (guess.length() > 1) {
                                System.out.println("Error! Please guess one letter.");
                                System.out.println();
                                System.out.print("Letter to guess: ");
                            } else if (guesses.contains(guess)){
                                System.out.println("Error! Letter " + guess + " has been guessed before, " +
                                        "please guess another letter.");
                                System.out.println();
                                System.out.print("Letter to guess: ");
                            } else {
                                guesses.add(guess);
                                validInput = true;
                            }
                        }
                    }

                    // create client message from valid user input
                    ClientMessage guessMessage = new ClientMessage(guess.length(), guess);
                    dos.write(Utils.convertToBytes(guessMessage));

                } else { // game message packet received
                    System.out.println(serverMessage.data);
                    gameOver = true;
                }
            }

        }

        scanner.close();
        dis.close();
        dos.close();
        gameServerSocket.close();

    }
} 