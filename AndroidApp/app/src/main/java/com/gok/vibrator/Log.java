package com.gok.vibrator;

public class Log {

    static final String TAG = "Vibrator";

    static void i(String msg) {
        android.util.Log.i(TAG, msg);
    }

    static void e(String msg, Exception e) {
        android.util.Log.e(TAG, msg, e);
    }
}
