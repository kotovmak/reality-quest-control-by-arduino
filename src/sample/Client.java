package sample;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Client /*extends Thread*/ implements Runnable {
    public Socket socket;
    public Boolean started;
    private BufferedReader in;
    private PrintWriter out;

    public Client() {
    }

    @Override
    public void run() {
        String line;
        while (started) {
            try {
                if (in != null && isConnected()) {
                    line = in.readLine();
                    System.out.println(line);
                } else {

                }
            } catch (IOException e) {
                Thread.currentThread().interrupt();
                e.printStackTrace();
            }
        }
    }

    public Boolean isConnected() {
        return socket != null && socket.isConnected();
    }

    public void sendln(String str) {
        if (isConnected()) if (out != null) out.println(str);
    }


    public String open(String ip, int port) {
        try {
            socket = new Socket();
            socket.connect(new InetSocketAddress(ip, port), 1000);
            socket.setTcpNoDelay(true);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out = new PrintWriter(socket.getOutputStream(), true);
            started = true;
            sendln("hi");
        } catch (Exception e) {
            StringWriter sw = new StringWriter();
            e.printStackTrace(new PrintWriter(sw));
            return sw.toString();
        }
        return null;
    }

    public Boolean close() {
        started = false;
        try {
            if (in != null) in = null;
            if (out != null) out = null;
            socket.shutdownInput();
            socket.shutdownOutput();
            socket.close();
            socket = null;
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }
}
