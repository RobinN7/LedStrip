package com.robin.ledstrips_bt;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.graphics.Color;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.larswerkman.holocolorpicker.ColorPicker;
import com.larswerkman.holocolorpicker.SaturationBar;
import com.larswerkman.holocolorpicker.ValueBar;

import org.jtransforms.fft.DoubleFFT_1D;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.UUID;

public class MainActivity extends Activity implements ColorPicker.OnColorChangedListener{

    protected int old_count = 0;
    protected int old_color = 0;

    protected short sData[] = new short[1024];
    protected int sDataAverage=0;
    protected double fftData[]= new double[1024];


    private static final String TAG = "LedStrips_BT";

    private static final int REQUEST_ENABLE_BT = 1;
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private OutputStream outStream = null;

    private AudioRecord audioInput = null;
    private Thread recordingThread = null;
    private boolean isRecording = false;

    // Well known SPP UUID
    private static final UUID MY_UUID =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    // Insert your bluetooth devices MAC address
    private static String address = "98:D3:31:30:43:2E";

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d(TAG, "In onCreate()");
        setContentView(R.layout.activity_main);

        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);

        // AudioRecord //
        /////////////////
        int RECORDER_CHANNELS = AudioFormat.CHANNEL_CONFIGURATION_MONO;
        int RECORDER_AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT;
        int RECORDER_SAMPLERATE= 44100;
        int bufferSize = AudioRecord.getMinBufferSize(RECORDER_SAMPLERATE, RECORDER_CHANNELS, RECORDER_AUDIO_ENCODING);
        audioInput = new AudioRecord(MediaRecorder.AudioSource.DEFAULT, RECORDER_SAMPLERATE, RECORDER_CHANNELS, RECORDER_AUDIO_ENCODING, bufferSize);

        // Fast Fourier Transform from JTransforms
        final DoubleFFT_1D fft = new DoubleFFT_1D(sData.length);


        // Start recording
        audioInput.startRecording();
        isRecording = true;
        recordingThread = new Thread(new Runnable() {
            public void run() {

                while (isRecording) {

                    // Record audio input
                    audioInput.read(sData, 0, sData.length);

                    // Convert and put sData short array into fftData double array to perform FFT
                    for (int j=0;j<sData.length;j++) {
                        fftData[j]=(double)sData[j];
                    }

                    // Perform 1D fft
                    fft.realForward(fftData);

                    sDataAverage=0;
                    for (int i=0; i<1024; i++) sDataAverage+=Math.abs(fftData[i]);
                    int attenuation=392; // Scaling factor to be in [0:255], to be replace with an adjustable value
                    sDataAverage/=1024*attenuation;

                    final int dataToSend = (sDataAverage > 255) ? 255 : sDataAverage; // Limits the value to 255

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            TextView tv = (TextView) findViewById(R.id.textView_debug);
                            tv.setText("Average amplitude : " + String.valueOf(dataToSend) + "\n"
                                    + "Fundamental index : " + "x");

                            sendData("R" + dataToSend + "G" + dataToSend + "B" + dataToSend);
                        }
                    });

                }
            }
        }, "AudioRecorder Thread");
        recordingThread.start();



        // Color Picker //
        //////////////////
        ColorPicker picker = (ColorPicker) findViewById(R.id.picker);
        SaturationBar saturationBar = (SaturationBar) findViewById(R.id.saturationbar);
        ValueBar valueBar = (ValueBar) findViewById(R.id.valuebar);
        picker.addSaturationBar(saturationBar);
        picker.addValueBar(valueBar);
        picker.setOnColorChangedListener(this);

        // Bluetooth //
        ///////////////
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        checkBTState();

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Set up a pointer to the remote node using it's address.
        BluetoothDevice device = btAdapter.getRemoteDevice(address);

        // Two things are needed to make a connection:
        //   A MAC address, which we got above.
        //   A Service ID or UUID.  In this case we are using the
        //     UUID for SPP.
        try {
            btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e) {
            errorExit("Fatal Error", "In onResume() and socket create failed: " + e.getMessage() + ".");
        }

        // Discovery is resource intensive.  Make sure it isn't going on
        // when you attempt to connect and pass your message.
        btAdapter.cancelDiscovery();

        // Establish the connection.  This will block until it connects.
        Log.d(TAG, "...Connecting to Remote...");
        try {
            btSocket.connect();
            Log.d(TAG, "...Connection established and data link opened...");
        } catch (IOException e) {
            try {
                btSocket.close();
            } catch (IOException e2) {
                errorExit("Fatal Error", "In onResume() and unable to close socket during connection failure" + e2.getMessage() + ".");
            }
        }

        // Create a data stream so we can talk to server.
        Log.d(TAG, "...Creating Socket...");

        try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            errorExit("Fatal Error", "In onResume() and output stream creation failed:" + e.getMessage() + ".");
        }
        ////////////////////////////////////////////////////////////////////////////////////////////



    }

    @Override
    public void onColorChanged(int color) {
        ColorPicker picker = (ColorPicker) findViewById(R.id.picker);
        picker.setOldCenterColor(color);

        int d = Math.abs(Color.green(color) - Color.green(old_color)) + Math.abs(Color.red(color) - Color.red(old_color)) + Math.abs(Color.blue(color) - Color.blue(old_color));
        if(d > 50 && old_count < 10)
        {
            old_count++;
            return;
        }
        old_count = 0;
        old_color = color;

        sendData("R" + Color.red(color) + "G" + Color.green(color) + "B" + Color.blue(color));
    }

    @Override
    public void onResume() {
        super.onResume();

        Log.d(TAG, "...In onResume()...");
        /*
        // Set up a pointer to the remote node using it's address.
        BluetoothDevice device = btAdapter.getRemoteDevice(address);

        // Two things are needed to make a connection:
        //   A MAC address, which we got above.
        //   A Service ID or UUID.  In this case we are using the
        //     UUID for SPP.
        try {
            btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e) {
            errorExit("Fatal Error", "In onResume() and socket create failed: " + e.getMessage() + ".");
        }

        // Discovery is resource intensive.  Make sure it isn't going on
        // when you attempt to connect and pass your message.
        btAdapter.cancelDiscovery();

        // Establish the connection.  This will block until it connects.
        Log.d(TAG, "...Connecting to Remote...");
        try {
            btSocket.connect();
            Log.d(TAG, "...Connection established and data link opened...");
        } catch (IOException e) {
            try {
                btSocket.close();
            } catch (IOException e2) {
                errorExit("Fatal Error", "In onResume() and unable to close socket during connection failure" + e2.getMessage() + ".");
            }
        }

        // Create a data stream so we can talk to server.
        Log.d(TAG, "...Creating Socket...");

        try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            errorExit("Fatal Error", "In onResume() and output stream creation failed:" + e.getMessage() + ".");
        }
        */
    }

    @Override
    public void onPause() {
        super.onPause();

        Log.d(TAG, "...In onPause()...");
        /*
        if (outStream != null) {
            try {
                outStream.flush();
            } catch (IOException e) {
                errorExit("Fatal Error", "In onPause() and failed to flush output stream: " + e.getMessage() + ".");
            }
        }

        try     {
            btSocket.close();
        } catch (IOException e2) {
            errorExit("Fatal Error", "In onPause() and failed to close socket." + e2.getMessage() + ".");
        }
        */
    }

    private void checkBTState() {
        // Check for Bluetooth support and then check to make sure it is turned on

        // Emulator doesn't support Bluetooth and will return null
        if(btAdapter==null) {
            errorExit("Fatal Error", "Bluetooth Not supported. Aborting.");
        } else {
            if (btAdapter.isEnabled()) {
                Log.d(TAG, "...Bluetooth is enabled...");
            } else {
                //Prompt user to turn on Bluetooth
                Intent enableBtIntent = new Intent(btAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }
        }
    }

    private void errorExit(String title, String message){
        Toast msg = Toast.makeText(getBaseContext(),
                title + " - " + message, Toast.LENGTH_SHORT);
        msg.show();
        finish();
    }

    private void sendData(String message) {
        byte[] msgBuffer = message.getBytes();

        Log.d(TAG, "...Sending data: " + message + "...");

        try {
            outStream.write(msgBuffer);
        } catch (IOException e) {
            /*
            String msg = "In onResume() and an exception occurred during write: " + e.getMessage();
            if (address.equals("00:00:00:00:00:00"))
                msg = msg + ".\n\nUpdate your server address from 00:00:00:00:00:00 to the correct address on line 37 in the java code";
            msg = msg +  ".\n\nCheck that the SPP UUID: " + MY_UUID.toString() + " exists on server.\n\n";

            errorExit("Fatal Error", msg);*/
        }
    }
}