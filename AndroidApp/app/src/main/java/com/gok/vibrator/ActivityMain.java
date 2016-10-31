package com.gok.vibrator;

import android.app.DownloadManager;
import android.content.BroadcastReceiver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import static android.R.attr.data;
import static android.R.attr.debuggable;

public class ActivityMain extends AppCompatActivity {

    static final String IP = "192.168.4.1";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        Log.i("starting");

        loadWifiAvailableList();

        Button pair = (Button) findViewById(R.id.pair);
        pair.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //new PostTask().execute(getMAC(), ((Spinner) findViewById(R.id.ssidList)).getSelectedItem().toString(), ((EditText) findViewById(R.id.password)).getText().toString());
                startActivity(new Intent(ActivityMain.this, ActivityControl.class));
            }
        });
    }

    public void loadWifiAvailableList() {
        WifiManager wifiManager = (WifiManager)
                getSystemService(Context.WIFI_SERVICE);
        List<ScanResult> results = wifiManager.getScanResults();
        String message = "No results. Check wireless is on";
        if (results != null) {
            final int size = results.size();
            if (size == 0) message = "No access points in range";
            else {
                Spinner spinner = (Spinner) findViewById(R.id.ssidList);
                List<String> list = new ArrayList<String>();
                for (ScanResult result : results) {
                    list.add(result.SSID);
                }
                ArrayAdapter<String> adp1=new ArrayAdapter<String>(this,
                        android.R.layout.simple_list_item_1,list);
                adp1.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                spinner.setAdapter(adp1);
                return;
            }
        }
        Toast.makeText(this, message, Toast.LENGTH_LONG).show();
    }

    public String getMAC() {
        WifiManager manager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = manager.getConnectionInfo();
        String address = info.getMacAddress();
        return address;
    }

    private class PostTask extends AsyncTask<String, String, Void> {

        @Override
        protected Void doInBackground(String... parameters) {
            send("mac", parameters[0].replace(":", ""));
            send("ssid", parameters[1]);
            send("passwd", parameters[2]);
            receiveKeys();
            return null;
        }

        protected void send(String method, String parameter) {
            get("http://"+IP+"/"+method+"?params="+parameter);
        }

        protected void receiveKeys() {
            String body = get("http://"+IP+"/key");
            try {
                JSONObject jsonObject = new JSONObject(body).getJSONObject("variables");
                SharedPreferences sharedPref = getPreferences(Context.MODE_PRIVATE);
                SharedPreferences.Editor editor = sharedPref.edit();
                editor.putString("Key", jsonObject.getString("key"));
                editor.putString("ID", jsonObject.getString("id"));
                editor.commit();
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        protected String get(String destination) {
            BufferedReader in = null;
            try {
                Log.i("Calling "+ destination);

                URL url = new URL(destination);
                HttpURLConnection client = (HttpURLConnection) url.openConnection();

                client.setConnectTimeout(1000*10);
                in = new BufferedReader(new InputStreamReader((client.getInputStream())));
                StringBuffer buffer = new StringBuffer();
                String line;
                while((line = in.readLine()) != null) {
                    buffer.append(line);
                }
                Log.i("Received "+ buffer);
                return buffer.toString();
            } catch (Exception e) {
                Log.e("Failed to send to "+destination, e);
            } finally {
                if(in != null) {
                    try {
                        in.close();
                    } catch (IOException e) {
                        Log.e("Failed to close the stream to "+destination, e);
                    }
                }
            }
            return "";
        }
    }

}
