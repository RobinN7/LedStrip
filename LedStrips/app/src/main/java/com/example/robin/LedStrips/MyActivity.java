package com.example.robin.LedStrips;

import android.graphics.Color;
import android.os.NetworkOnMainThreadException;
import android.os.StrictMode;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.larswerkman.holocolorpicker.ColorPicker;
import com.larswerkman.holocolorpicker.SaturationBar;
import com.larswerkman.holocolorpicker.ValueBar;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;

public class MyActivity extends ActionBarActivity  implements ColorPicker.OnColorChangedListener {

    protected Socket socket;

    protected int old_count = 0;
    protected int old_color = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my);

        ColorPicker picker = (ColorPicker) findViewById(R.id.picker);
        SaturationBar saturationBar = (SaturationBar) findViewById(R.id.saturationbar);
        ValueBar valueBar = (ValueBar) findViewById(R.id.valuebar);

        picker.addSaturationBar(saturationBar);
        picker.addValueBar(valueBar);
        picker.setOnColorChangedListener(this);

        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);
    }

    public void openSocket(View view) {
        TextView tv = (TextView) findViewById(R.id.textView_debug);

        EditText editText = (EditText) findViewById(R.id.edit_ip_adress);
        String ip_address = editText.getText().toString();

        try {
            socket = new Socket();
            socket.connect(new InetSocketAddress(InetAddress.getByName(ip_address), 22188));
            tv.setText("Socket opened on " + ip_address + " !");
            //} catch(IOException | android.os.NetworkOnMainThreadException ex)
        } catch(Exception ex)
        {
            tv.setText("Unable to open socket on " + ip_address + " !\n" + ex.toString());
        }
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

        writeSocket(new byte[]{'R', (byte) Color.red(color), 'G', (byte) Color.green(color), 'B', (byte) Color.blue(color)});
    }

    private void writeSocket(byte[] b) {
        try {
            socket.getOutputStream().write(b);
            TextView tv = (TextView) findViewById(R.id.textView_debug);
            tv.setText(":)");
        } catch(NullPointerException | IOException | NetworkOnMainThreadException ex)
        {
            TextView tv = (TextView) findViewById(R.id.textView_debug);
            tv.setText("Unable to write to the socket !" + ex.toString());
        }
    }

    public void sendClose(View view) {
        writeSocket(new byte[]{'C'});
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_my, menu);
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
}
