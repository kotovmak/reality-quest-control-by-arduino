package sample;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.util.Duration;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;

public class Controller {
    public int sec = 0;
    public int min = 0;
    @FXML
    public TextField txtip;
    public TextField txtport;
    public Button connect;
    public Button st;
    public Label time;
    public Rectangle r0;
    public Rectangle r1;
    public Rectangle r2;
    public Rectangle r3;
    public Rectangle r4;
    public Rectangle r5;
    public Rectangle r6;
    public Rectangle r7;
    public Rectangle r8;
    public Rectangle r9;
    public Rectangle r10;
    public Rectangle r11;
    public Rectangle r12;
    public Rectangle r13;
    public Rectangle r14;
    public Rectangle r15;
    public TextArea console;
    public TextArea console1;
    public ComboBox tracks;
    Thread thread;
    private Client client = new Client();
    private PrintStream ps;
    private String err;
    private String filesPath = "C:\\0\\";
    private String playerParams = "--play-and-exit";
    private String playerPath = "c:\\Program Files\\VideoLAN\\VLC\\vlc.exe";

    private Timeline timeline = new Timeline(
            new KeyFrame(
                    Duration.millis(1000), //1000 мс * 60 сек = 1 мин
                    ae -> {
                        if (sec == 59) {
                            min++;
                            sec = 0;
                        }
                        sec++;
                        time.setText((min < 10 ? "0" + min : min) + ":" + (sec < 10 ? "0" + sec : sec));
                    }
            )
    );

    public void initialize() {
        ps = new PrintStream(new Console(console1));
    }

    public void st() {
        min = 0;
        sec = 0;
        time.setText("00:00");
        st.setText("Finish");
        timeline.setCycleCount(120000); //Ограничим число повторений
        timeline.play(); //Запускаем
    }

    public void reset() {
        st.setText("Start");
        timeline.stop();
    }

    public void butStClick() {
        if (client.isConnected())
            if (st.getText().equals("Start")) {
                //st();
                console.appendText("S\n");
                client.sendln("S");
            } else {
                //reset();
                console.appendText("AAAAAAAE\n");
                client.sendln("AAAAAAAE");
            }
    }

    public void butFClick() {
        if (client.isConnected()) {
            //reset();
            console.appendText("R\n");
            client.sendln("R");
        }
    }

    public void playTrack() {
        if (!tracks.getItems().isEmpty())
            play(tracks.getSelectionModel().getSelectedItem().toString());
    }

    public void getTracks() {
        File aDirectory = new File(filesPath);
        String[] filesInDir = aDirectory.list();
        tracks.getItems().clear();
        for (int i = 0; i < filesInDir.length; i++) {
            int k = filesInDir[i].lastIndexOf('.');
            if (k > 0) {
                tracks.getItems().add(filesInDir[i]);
            }
        }
    }

    public void play(String filename) {
        int k = filename.lastIndexOf('.');
        if (k > 0) {
            try {
                Process p = Runtime.getRuntime().exec(new String[]{playerPath, playerParams, filesPath + filename});
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void butClick(ActionEvent actionEvent) {
        if (client.isConnected()) {
            String id = ((Control) actionEvent.getSource()).getId();
            console.appendText(id + "\n");
            client.sendln(id);
        }
    }

    public void butConnectClick() {
        System.setOut(ps);
        System.setErr(ps);
        if (connect.getText().equals("Connect")) {
            console.appendText("connecting...\n");
            String ip = txtip.getText();
            String port = txtport.getText();
            if (ip.length() < 1) {
                console.appendText("ip не указан\n");
            } else if (port.length() < 1) {
                console.appendText("port не указан\n");
            } else {
                String err = client.open(ip, Integer.parseInt(port));
                if (err == null) {
                    console.appendText("connected\n");
                    connect.setText("Disconnect");
                    thread = new Thread(client);
                    thread.start();
                } else if (err.indexOf("connect timed out") > -1) {
                    console.appendText("connect timed out\n");
                } else {
                    console.appendText(err);
                }
            }
        } else {
            thread.stop();
            if (client.close()) {
                console.appendText("disconnected\n");
                connect.setText("Connect");
            }
        }
    }

    public void butClearClick() {
        console.clear();
        console1.clear();
    }

    public void butPingClick() {
        if (client.isConnected()) {
            console.appendText("ping\n");
            client.sendln("ping");
        }
    }

    public class Console extends OutputStream {
        private StringBuilder buffer;
        private TextArea console;


        public Console(TextArea console) {
            this.console = console;
            buffer = new StringBuilder(128);
        }

        public void appendText(String valueOf) {
            if (valueOf.indexOf("S") == 0) {
                Platform.runLater(() -> {
                    st();
                });
            } else if (valueOf.indexOf("R") == 0) {
                Platform.runLater(() -> {
                    reset();
                });
            } else if (valueOf.indexOf("T") == 0) {
                Platform.runLater(() -> {
                    st();
                });
            } else if (valueOf.indexOf("P") == 0) {
                String track = valueOf.substring(0, valueOf.length() - 2);
                play(track);
            }
            if (valueOf.length() > 17) if (valueOf.charAt(17) == 'T') Platform.runLater(() -> reset());
            for (int i = 0; i < valueOf.length() - 1; i++) {
                if (valueOf.charAt(i + 1) == 'T') {
                    if (i == 0) r0.setFill(Color.RED);
                    if (i == 1) r1.setFill(Color.RED);
                    if (i == 2) r2.setFill(Color.RED);
                    if (i == 3) r3.setFill(Color.RED);
                    if (i == 4) r4.setFill(Color.RED);
                    if (i == 5) r5.setFill(Color.RED);
                    if (i == 6) r6.setFill(Color.RED);
                    if (i == 7) r7.setFill(Color.RED);
                    if (i == 8) r8.setFill(Color.RED);
                    if (i == 9) r9.setFill(Color.RED);
                    if (i == 10) r10.setFill(Color.RED);
                    if (i == 11) r11.setFill(Color.RED);
                    if (i == 12) r12.setFill(Color.RED);
                    if (i == 13) r13.setFill(Color.RED);
                    if (i == 14) r14.setFill(Color.RED);
                    if (i == 15) r15.setFill(Color.RED);
                } else if (valueOf.charAt(i + 1) == 'F') {
                    if (i == 0) r0.setFill(Color.DODGERBLUE);
                    if (i == 1) r1.setFill(Color.DODGERBLUE);
                    if (i == 2) r2.setFill(Color.DODGERBLUE);
                    if (i == 3) r3.setFill(Color.DODGERBLUE);
                    if (i == 4) r4.setFill(Color.DODGERBLUE);
                    if (i == 5) r5.setFill(Color.DODGERBLUE);
                    if (i == 6) r6.setFill(Color.DODGERBLUE);
                    if (i == 7) r7.setFill(Color.DODGERBLUE);
                    if (i == 8) r8.setFill(Color.DODGERBLUE);
                    if (i == 9) r9.setFill(Color.DODGERBLUE);
                    if (i == 10) r10.setFill(Color.DODGERBLUE);
                    if (i == 11) r11.setFill(Color.DODGERBLUE);
                    if (i == 12) r12.setFill(Color.DODGERBLUE);
                    if (i == 13) r13.setFill(Color.DODGERBLUE);
                    if (i == 14) r14.setFill(Color.DODGERBLUE);
                    if (i == 15) r15.setFill(Color.DODGERBLUE);
                }
            }
            Platform.runLater(() -> {
                console.appendText(valueOf);
            });
        }

        @Override
        public void write(int b) {
            char c = (char) b;
            String value = Character.toString(c);
            buffer.append(value);
            if (value.equals("\n")) {
                appendText(buffer.toString());
                buffer.delete(0, buffer.length());
            }
        }
    }
}
