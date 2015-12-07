package com.example.kevin.bluetoothtest2;

import android.app.TimePickerDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.Handler;
import android.support.v4.app.FragmentActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.widget.TimePicker;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.Format;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.UUID;

import android.widget.Button;
import android.widget.TextView;


public class MainActivity extends FragmentActivity implements TimePickerDialog.OnTimeSetListener{

    BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    android.bluetooth.BluetoothSocket btSocket;

    private static final String TAG = "kev";
    private OutputStream outStream = null;
    private InputStream inStream = null;
    Handler  handler = new Handler();
    byte delimiter = 10;
    boolean stopWorker = false;
    int readBufferPosition = 0;
    byte[] readBuffer = new byte[1024];

    TextView receivedMessage;
    Button setAlarmButton, setTimeButton, getTimeButton, getAlarmButton, clearLogButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        receivedMessage = (TextView) findViewById(R.id.receivedMessage);
        setAlarmButton = (Button) findViewById(R.id.setAlarmButton);
        setTimeButton = (Button) findViewById(R.id.setTimeButton);
        getTimeButton = (Button) findViewById(R.id.getTimeButton);
        getAlarmButton = (Button) findViewById(R.id.getAlarmButton);
        clearLogButton = (Button) findViewById(R.id.clearLogButton);

        setTimeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Calendar c = Calendar.getInstance();

                new TimePickerDialog(MainActivity.this, new TimePickerDialog.OnTimeSetListener() {

                    @Override
                    public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                        receivedMessage.setText("Time: " + hourOfDay + " : " + minute);
                        writeData("1, " + hourOfDay + ", " + minute +", 0, 1, 1, 1");
                    }
                },
                        c.get(Calendar.HOUR_OF_DAY),
                        c.get(Calendar.MINUTE),
                        true).show();
            }
        });

        setAlarmButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Calendar c = Calendar.getInstance();

                new TimePickerDialog(MainActivity.this, new TimePickerDialog.OnTimeSetListener() {

                    @Override
                    public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                        receivedMessage.setText("Time: " + hourOfDay + " : " + minute);
                        writeData("3, " + hourOfDay + ", " + minute +", 0");
                    }
                },
                        c.get(Calendar.HOUR_OF_DAY),
                        c.get(Calendar.MINUTE),
                        true).show();
            }
        });
        getAlarmButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                writeData("4");
            }
        });
        getTimeButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                writeData("2");
            }
        });

        clearLogButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                receivedMessage.setText("---");
            }
        });
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

    @Override
    public void onTimeSet(TimePicker view, int hour, int minute)
    {

        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.HOUR, hour);
        calendar.set(Calendar.MINUTE, minute);

        Format formatter = new SimpleDateFormat("HH:mm:ss");
        String s = formatter.format(calendar.getTime());

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

        beginListenForData();
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

    public void beginListenForData()   {
        try {
            inStream = btSocket.getInputStream();
        } catch (IOException e) {
        }

        Thread workerThread = new Thread(new Runnable()
        {
            public void run()
            {
                while(!Thread.currentThread().isInterrupted() && !stopWorker)
                {
                    try
                    {
                        int bytesAvailable = inStream.available();
                        if(bytesAvailable > 0)
                        {
                            byte[] packetBytes = new byte[bytesAvailable];
                            inStream.read(packetBytes);
                            for(int i=0;i<bytesAvailable;i++)
                            {
                                byte b = packetBytes[i];
                                if(b == delimiter)
                                {
                                    byte[] encodedBytes = new byte[readBufferPosition];
                                    System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                    final String data = new String(encodedBytes, "US-ASCII");
                                    readBufferPosition = 0;
                                    handler.post(new Runnable()
                                    {
                                        public void run()
                                        {

                                            if(receivedMessage.getText().toString().equals("..")) {
                                                receivedMessage.setText(data);
                                            } else {
                                                receivedMessage.append("\n" + data);
                                            }

	                                        	/* You also can use Result.setText(data); it won't display multilines
	                                        	*/

                                        }
                                    });
                                }
                                else
                                {
                                    readBuffer[readBufferPosition++] = b;
                                }
                            }
                        }
                    }
                    catch (IOException ex)
                    {
                        stopWorker = true;
                    }
                }
            }
        });

        workerThread.start();
    }

}
