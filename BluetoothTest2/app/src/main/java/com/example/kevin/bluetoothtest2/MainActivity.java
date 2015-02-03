package com.example.kevin.bluetoothtest2;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;


public class MainActivity extends ActionBarActivity {

    BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    android.bluetooth.BluetoothSocket btSocket;

    private static final String TAG = "kev";
    private OutputStream outStream = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        CheckBt();
        Connect();
        writeData("13");
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }


    private void CheckBt() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if (!mBluetoothAdapter.isEnabled()) {
            Toast.makeText(getApplicationContext(), "Bluetooth Disabled !",
                    Toast.LENGTH_SHORT).show();
                   /* It tests if the bluetooth is enabled or not, if not the app will show a message. */
        }

        if (mBluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(),
                    "Bluetooth null !", Toast.LENGTH_SHORT)
                    .show();
        }
    }


    public void Connect() { //ac:22:0b:3e:19:79 DD:17:36:C6:28:83
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice("20:14:05:15:19:04");
        Log.d("", "Connecting to ... " + device);
        mBluetoothAdapter.cancelDiscovery();
        try {
            btSocket = device.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
/* Here is the part the connection is made, by asking the device to create a RfcommSocket (Unsecure socket I guess), It map a port for us or something like that */
            btSocket.connect();
            Log.d("", "Connection made.");
        } catch (IOException e) {
            try {
                btSocket.close();
            } catch (IOException e2) {
                Log.d("", "Unable to end the connection");
            }
            Log.d("", "Socket creation failed");
        }

      //  beginListenForData();
               /* this is a method used to read what the Arduino says for example when you write Serial.print("Hello world.") in your Arduino code */
    }

    private void writeData(String data) {
        try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            Log.d(TAG, "Bug AVANT l'envoie.", e);
        }

        String message = data;

        byte[] msgBuffer = message.getBytes();

        try {
            outStream.write(msgBuffer);
        } catch (IOException e) {
            Log.d(TAG, "Bug DURANT l'envoie.", e);
        }
    }



}
