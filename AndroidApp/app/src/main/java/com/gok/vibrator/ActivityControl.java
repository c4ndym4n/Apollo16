package com.gok.vibrator;

import android.os.AsyncTask;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.RadioButton;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.URL;

public class ActivityControl extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        new AsyncTask<Void, Void, Void>() {

            @Override
            public Void doInBackground(Void... voids) {
                try {
//                    Thread.sleep(1000*4);
                    Thread.sleep(0);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                return null;
            }

            @Override
            protected void onPostExecute(Void aVoid) {
                onConnected();
            }
        }.execute();

        setupRadioButton(R.id.speed0, 0);
        setupRadioButton(R.id.speed1, 1);
        setupRadioButton(R.id.speed2, 2);
        setupRadioButton(R.id.speed3, 3);
        setupRadioButton(R.id.speed4, 4);
    }

    protected void setupRadioButton(int ID, final int speed) {
        ((RadioButton) findViewById(ID)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new ChangeState().execute(speed);
            }
        });
    }

    protected void onConnected() {
        findViewById(R.id.connectingLabel).setVisibility(View.GONE);
        findViewById(R.id.connectingProgress).setVisibility(View.GONE);
        findViewById(R.id.controllView).setVisibility(View.VISIBLE);
    }

    private static class ChangeState extends AsyncTask<Integer, Void, Void> {

        private static final String KEY = "***********************";
        private static final String IP = "https://io.adafruit.com/api/feeds/onoff/data";

        @Override
        protected Void doInBackground(Integer... params) {
            try {
                HttpURLConnection connection = (HttpURLConnection) new URL(IP).openConnection();
                String payload = "{\n\"value\": \""+(params[0] > 0 ? "ON" : "OFF")+"\"\n}";

                connection.setDoOutput(true);
                connection.setDoInput(true);
                connection.setRequestMethod("POST");
                connection.setRequestProperty("Accept", "application/json");
                connection.setRequestProperty("Content-Type", "application/json");
                connection.setRequestProperty("Content-Length", payload.length()+"");
                connection.setRequestProperty("X-AIO-Key", KEY);

                OutputStreamWriter writer = new OutputStreamWriter(connection.getOutputStream());
                writer.write(payload);
                writer.close();

                int returnCode = connection.getResponseCode();

                if(returnCode == 200 || returnCode == 201) {
                    BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));
                    StringBuffer buffer = new StringBuffer();
                    String line = null;
                    while ((line = reader.readLine()) != null) {
                        buffer.append(line);
                    }
                    Log.i("Received: "+buffer.toString());
                } else {
                    BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getErrorStream()));
                    StringBuffer buffer = new StringBuffer();
                    String line = null;
                    while ((line = reader.readLine()) != null) {
                        buffer.append(line);
                    }
                    throw new RuntimeException(buffer.toString());
                }
            } catch(MalformedURLException e) {
                e.printStackTrace();
            } catch(Exception e) {
                e.printStackTrace();
            }
            return null;
        }
    }

}
