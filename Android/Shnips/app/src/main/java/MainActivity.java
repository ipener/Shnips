// Copyright © 2018 Igor Pener. All rights reserved.

package shnips;

import android.app.AlarmManager;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import org.apache.commons.io.IOUtils;
import static android.app.PendingIntent.FLAG_CANCEL_CURRENT;

public class MainActivity extends AppCompatActivity {
    private static final int SET_PAUSED = 0;
    private static final int SET_ACHIEVEMENTS_PROGRESS = 1;
    private static final int SET_LEADERBOARD_DATA = 2;
    private static final int GET_LEADERBOARD_DATA = 3;
    private static final int SHOW_APP_RATING_VIEW = 4;
    private static final int SHOW_TWITTER_WEBSITE = 5;
    private static final int SHOW_ACHIEVEMENTS = 6;
    private static final int SHOW_LEADERBOARD = 7;
    private static final int START_PURCHASE = 8;
    private static final int SHARE = 9;
    private static final int SCHEDULE_NOTIFICATIONS = 10;
    private static final int CANCEL_NOTIFICATIONS = 11;
    private static final int SHOW_QUIT_APP_ALERT = 12;
    private static final int SET_UP_IN_APP_BILLING = 13;
    private static final int RANGE = 14;

    private static final String TAG = "MainActivity";
    private static final String NOTIFICATION_ID = "id";
    private static final String NOTIFICATION_TITLE = "title";
    private static final String NOTIFICATION_DELAY = "delay";

    static {
        System.loadLibrary("c++_shared");
        System.loadLibrary("shnips-core-jni");
    }

    private static MainActivity self;

    private GLView mGLView;
    private GooglePlayStore mGooglePlayStore;
    private GooglePlayGames mGooglePlayGames;
    private AppDelegate mAppDelegate;
    private long mAppResumeTime = 0;
    private short mNotificationId = -1;
    private Callback[] mCallbacks = new Callback[RANGE];

    // Calls to C++ GameController
    public static native void init(float width, float height, float scale_factor);

    public static native void dealloc();

    public static native void appDidBecomeActive();

    public static native void appWillResignActive();

    public static native void update(float t);

    public static native void touchesBegan(double x, double y, long t);

    public static native void touchesMoved(double x, double y, long t);

    public static native void touchesEnded(double x, double y, long t);

    public static native void didRetrieveRank(long rank, short index);

    public static native void didRetrievePurchase(String price, short type);

    public static native void didBuyInAppPurchase(short type);

    public static native void didCancelInAppPurchase();

    public static native void setAuthenticated(boolean authenticated);

    public static native void didTapBackButton();

    private interface Callback {
        void execute(Bundle data);
    }

    private class AppDelegate extends Handler {
        @Override
        public synchronized void handleMessage(Message msg) {
            if (msg.what >= 0 && msg.what < RANGE) {
                mCallbacks[msg.what].execute(msg.getData());
            } else {
                super.handleMessage(msg);
            }
        }
    }

    public static class AlarmReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final long []delays = intent.getLongArrayExtra(NOTIFICATION_DELAY);
            final String []titles = intent.getStringArrayExtra(NOTIFICATION_TITLE);
            final short []ids = intent.getShortArrayExtra(NOTIFICATION_ID);

            if (delays != null && titles != null && ids != null && delays.length > 0 && titles.length > 0 && ids.length > 0) {
                PendingIntent pendingIntent = TaskStackBuilder.create(context)
                    .addParentStack(MainActivity.class)
                    .addNextIntent(new Intent(context, MainActivity.class).putExtra(NOTIFICATION_ID, ids[0]))
                    .getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);

                NotificationCompat.Builder builder = new NotificationCompat.Builder(context)
                    .setSmallIcon(R.drawable.android_notification)
                    .setLargeIcon(BitmapFactory.decodeResource(context.getResources(), R.mipmap.ic_launcher))
                    .setContentTitle(titles[0])
                    .setContentText(context.getString(R.string.app_name))
                    .setAutoCancel(true)
                    .setContentIntent(pendingIntent);

                NotificationManager manager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
                manager.notify(0, builder.build());

                if (delays.length > 1 && titles.length > 1 && ids.length > 1) {
                    long []newDelays = Arrays.copyOfRange(delays, 1, delays.length);
                    String []newTitles = Arrays.copyOfRange(titles, 1, titles.length);
                    short []newIds = Arrays.copyOfRange(ids, 1, ids.length);
                    self.scheduleNotifications(newDelays, newTitles, newIds);
                }
            }
        }
    }

    public static void queueEvent(Runnable runnable) {
        self.mGLView.queueEvent(runnable);
    }

    private static SharedPreferences sharedPreferences() {
        return self.getSharedPreferences(self.getLocalClassName(), Context.MODE_PRIVATE);
    }

    private static long delayForDays(short days) {
        return self.mAppResumeTime + AlarmManager.INTERVAL_DAY * days;
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLView.queueEvent(new Runnable() {
            @Override
            public void run() {
                MainActivity.appWillResignActive();
            }
        });
        mGLView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        setUpWindow();
        mGLView.onResume();
        MainActivity.appDidBecomeActive();
    }

    @Override
    protected void onStart() {
        super.onStart();
        if (mGooglePlayGames != null) {
            mGooglePlayGames.onStart();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (mGooglePlayGames != null) {
            mGooglePlayGames.onStop();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mGLView = new GLView(getApplication());
        setContentView(mGLView);

        mNotificationId = -1;
        Intent intent = getIntent();
        if (intent.hasExtra(NOTIFICATION_ID)) {
            mNotificationId = intent.getShortExtra(NOTIFICATION_ID, (short)-1);
        }

        // Setting up all callbacks
        mCallbacks[SET_PAUSED] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                setPaused(bundle);
            }
        };
        mCallbacks[SET_ACHIEVEMENTS_PROGRESS] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                setAchievementsProgress(bundle);
            }
        };
        mCallbacks[SET_LEADERBOARD_DATA] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                setLeaderboardData(bundle);
            }
        };
        mCallbacks[GET_LEADERBOARD_DATA] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                getLeaderboardData(bundle);
            }
        };
        mCallbacks[SHOW_APP_RATING_VIEW] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                showAppRatingView(bundle);
            }
        };
        mCallbacks[SHOW_TWITTER_WEBSITE] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                showTwitterWebsite(bundle);
            }
        };
        mCallbacks[SHOW_ACHIEVEMENTS] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                showAchievements(bundle);
            }
        };
        mCallbacks[SHOW_LEADERBOARD] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                showLeaderboard(bundle);
            }
        };
        mCallbacks[START_PURCHASE] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                startPurchase(bundle);
            }
        };
        mCallbacks[SHARE] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                share(bundle);
            }
        };
        mCallbacks[SCHEDULE_NOTIFICATIONS] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                scheduleNotifications(bundle);
            }
        };
        mCallbacks[CANCEL_NOTIFICATIONS] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                cancelNotifications(bundle);
            }
        };
        mCallbacks[SHOW_QUIT_APP_ALERT] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                showQuitAppAlert(bundle);
            }
        };
        mCallbacks[SET_UP_IN_APP_BILLING] = new Callback() {
            @Override
            public void execute(Bundle bundle) {
                setUpInAppBilling(bundle);
            }
        };

        mGooglePlayGames = new GooglePlayGames(this);
        mAppDelegate = new AppDelegate();
        mAppResumeTime = SystemClock.elapsedRealtime();
        self = this;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mGooglePlayStore != null) {
            mGooglePlayStore.onDestroy();
        }
        MainActivity.dealloc();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            setUpWindow();
        }
    }

    @Override
    public void onBackPressed() {
        if (mGLView != null) {
            mGLView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    MainActivity.didTapBackButton();
                }
            });
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == GooglePlayStore.PURCHASE_REQUEST) {
            if (mGooglePlayStore != null) {
                mGooglePlayStore.onActivityResult(requestCode, resultCode, data);
            }
        } else if (mGooglePlayGames != null) {
            mGooglePlayGames.onActivityResult(requestCode, resultCode);
        }
    }

    public static void reset() {
        if (self != null) {
            sharedPreferences().edit().clear().commit();
        }
    }

    public static void save(String data, String key) {
        if (self != null) {
            if (sharedPreferences() != null && key != null && data != null) {
                SharedPreferences.Editor editor = sharedPreferences().edit();
                editor.putString(key, data);
                editor.apply();
            }
        }
    }

    public static String load(String key) {
        if (self != null && sharedPreferences() != null && key != null)
            return sharedPreferences().getString(key, null);

        return null;
    }

    public static String localizedString(String key) {
        if (self == null)
            return null;

        return self.getString(self.getResources().getIdentifier(key, "string", self.getPackageName()));
    }

    public static Bitmap loadImage(String filename) {
        AssetManager mgr = null;
        if (self != null) {
            mgr = self.getApplication().getAssets();
        }
        if (mgr != null) {
            try {
                InputStream stream = mgr.open(filename);
                Bitmap bitmap = BitmapFactory.decodeStream(stream);
                stream.close();
                return bitmap;
            } catch (Exception e) {
                Log.e(TAG, "Failed to read file: " + filename + " received exception:" + e.getMessage());
            }
        }
        return null;
    }

    public static byte[] loadAudio(String filename) {
        AssetManager mgr = null;
        if (self != null) {
            mgr = self.getApplication().getAssets();
        }
        if (mgr != null) {
            try {
                InputStream stream = mgr.open(filename);
                byte[] buff = IOUtils.toByteArray(stream);
                stream.close();
                return buff;
            } catch (Exception e) {
                Log.e(TAG, "Failed to read file: " + filename + " received exception:" + e.getMessage());
            }
        }
        return null;
    }

    public static void setPaused(boolean paused) {
        if (self != null) {
            Message msg = self.mAppDelegate.obtainMessage(SET_PAUSED);
            Bundle bundle = new Bundle();
            bundle.putBoolean("paused", paused);
            msg.setData(bundle);
            self.mAppDelegate.sendMessage(msg);
        }
    }

    public static void setAchievementsProgress(float[] progress) {
        if (self != null) {
            Message msg = self.mAppDelegate.obtainMessage(SET_ACHIEVEMENTS_PROGRESS);
            Bundle bundle = new Bundle();
            bundle.putFloatArray("progress", progress);
            msg.setData(bundle);
            self.mAppDelegate.sendMessage(msg);
        }
    }

    public static void setLeaderboardData(long score) {
        if (self != null) {
            Message msg = self.mAppDelegate.obtainMessage(SET_LEADERBOARD_DATA);
            Bundle bundle = new Bundle();
            bundle.putLong("score", score);
            msg.setData(bundle);
            self.mAppDelegate.sendMessage(msg);
        }
    }

    public static void getLeaderboardData() {
        if (self != null) {
            self.mAppDelegate.sendMessage(self.mAppDelegate.obtainMessage(GET_LEADERBOARD_DATA));
        }
    }

    public static void showAppRatingView() {
        if (self != null) {
            self.mAppDelegate.sendMessage(self.mAppDelegate.obtainMessage(SHOW_APP_RATING_VIEW));
        }
    }

    public static void showTwitterWebsite() {
        if (self != null) {
            self.mAppDelegate.sendMessage(self.mAppDelegate.obtainMessage(SHOW_TWITTER_WEBSITE));
        }
    }

    public static void showAchievements() {
        if (self != null) {
            self.mAppDelegate.sendMessage(self.mAppDelegate.obtainMessage(SHOW_ACHIEVEMENTS));
        }
    }

    public static void showLeaderboard() {
        if (self != null) {
            self.mAppDelegate.sendMessage(self.mAppDelegate.obtainMessage(SHOW_LEADERBOARD));
        }
    }

    public static boolean authenticatedForLeaderboard() {
        return self != null && self.mGooglePlayGames.authenticated();
    }

    public static void startPurchase(short type) {
        if (self != null) {
            Message msg = self.mAppDelegate.obtainMessage(START_PURCHASE);
            Bundle bundle = new Bundle();
            bundle.putShort("type", type);
            msg.setData(bundle);
            self.mAppDelegate.sendMessage(msg);
        }
    }

    public static void share(String str) {
        if (self != null) {
            Message msg = self.mAppDelegate.obtainMessage(SHARE);
            Bundle bundle = new Bundle();
            bundle.putString("str", str);
            msg.setData(bundle);
            self.mAppDelegate.sendMessage(msg);
        }
    }

    public static void scheduleNotifications(String[] titles) {
        if (self != null) {
            Message msg = self.mAppDelegate.obtainMessage(SCHEDULE_NOTIFICATIONS);
            Bundle bundle = new Bundle();
            bundle.putStringArray("titles", titles);
            msg.setData(bundle);
            self.mAppDelegate.sendMessage(msg);
        }
    }

    public static void cancelNotifications() {
        if (self != null) {
            self.mAppDelegate.sendMessage(self.mAppDelegate.obtainMessage(CANCEL_NOTIFICATIONS));
        }
    }

    public static void showQuitAppAlert() {
        if (self != null) {
            self.mAppDelegate.sendMessage(self.mAppDelegate.obtainMessage(SHOW_QUIT_APP_ALERT));
        }
    }

    public static void setUpInAppBilling() {
        if (self != null) {
            self.mAppDelegate.sendMessage(self.mAppDelegate.obtainMessage(SET_UP_IN_APP_BILLING));
        }
    }

    private void setPaused(Bundle bundle) {
        if (bundle != null) {
            boolean paused = bundle.getBoolean("paused");
            if (mGLView != null) {
                mGLView.setRenderMode(paused ? GLSurfaceView.RENDERMODE_WHEN_DIRTY : GLSurfaceView.RENDERMODE_CONTINUOUSLY);
            }
        }
    }

    private void setAchievementsProgress(Bundle bundle) {
        if (bundle != null) {
            final float[] progress = bundle.getFloatArray("progress");
            if (mGooglePlayGames != null && progress != null) {
                mGooglePlayGames.setAchievementsProgress(progress);
            }
        }
    }

    private void setLeaderboardData(Bundle bundle) {
        if (bundle != null) {
            long score = bundle.getLong("score");
            if (mGooglePlayGames != null) {
                mGooglePlayGames.reportScore(score);
            }
        }
    }

    private void getLeaderboardData(Bundle bundle) {
        if (bundle != null) {
            if (mGooglePlayGames != null) {
                mGooglePlayGames.loadLeaderboardData();
            }
        }
    }

    private void showAppRatingView(Bundle bundle) {
        if (bundle != null) {
            final Uri uri = Uri.parse("market://details?id=" + getApplicationContext().getPackageName());
            final Intent intent = new Intent(Intent.ACTION_VIEW, uri);
            if (getPackageManager().queryIntentActivities(intent, 0).size() > 0) {
                startActivity(intent);
            }
        }
    }

    private void showTwitterWebsite(Bundle bundle) {
        if (bundle != null) {
            final Uri uri = Uri.parse("https://twitter.com/shnipsgame");
            final Intent intent = new Intent(Intent.ACTION_VIEW, uri);
            if (getPackageManager().queryIntentActivities(intent, 0).size() > 0) {
                startActivity(intent);
            }
        }
    }

    private void showAchievements(Bundle bundle) {
        if (bundle != null) {
            if (mGooglePlayGames != null) {
                mGooglePlayGames.showAchievements();
            }
        }
    }

    private void showLeaderboard(Bundle bundle) {
        if (bundle != null) {
            if (mGooglePlayGames != null) {
                mGooglePlayGames.showLeaderboard();
            }
        }
    }

    private void startPurchase(Bundle bundle) {
        if (bundle != null) {
            short type = bundle.getShort("type");
            if (mGooglePlayStore != null) {
                mGooglePlayStore.startPurchase(type);
            }
        }
    }

    private void share(Bundle bundle) {
        if (bundle != null) {
            String str = bundle.getString("str");
            Intent intent = new Intent()
                .setAction(Intent.ACTION_SEND)
                .putExtra(Intent.EXTRA_TEXT, str)
                .setType("text/plain");
            startActivity(Intent.createChooser(intent, getString(R.string.share_score)));
        }
    }

    private synchronized void scheduleNotifications(Bundle bundle) {
        if (bundle != null) {
            final String[] titles = bundle.getStringArray("titles");
            ArrayList<Long> delayList = new ArrayList<Long>();
            ArrayList<String> titleList = new ArrayList<String>();
            ArrayList<Short> idList = new ArrayList<Short>();

            for (short i = 0, days = 1; i < titles.length; ++i) {
                if (titles[i] != null) {
                    delayList.add(delayForDays(days));
                    titleList.add(titles[i]);
                    idList.add(i);
                    days <<= 1;
                }
            }
            if (delayList.size() > 0 && delayList.size() == titleList.size()) {
                long[] newDelays = new long[delayList.size()];
                String[] newTitles = new String[titleList.size()];
                short[] newIds = new short[idList.size()];
                for (int i = 0; i < delayList.size(); ++i) {
                    newDelays[i] = delayList.get(i);
                    newTitles[i] = titleList.get(i);
                    newIds[i] = idList.get(i);
                }
                scheduleNotifications(newDelays, newTitles, newIds);
            }
        }
    }

    private void cancelNotifications(Bundle bundle) {
        if (bundle != null) {
            Context context = getApplicationContext();
            Intent intent = new Intent(context, shnips.MainActivity.AlarmReceiver.class);
            AlarmManager manager = (AlarmManager)context.getSystemService(Context.ALARM_SERVICE);
            manager.cancel(PendingIntent.getBroadcast(context, 0, intent, FLAG_CANCEL_CURRENT));

            NotificationManager notificationManager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
            notificationManager.cancelAll();
        }
    }

    private void showQuitAppAlert(Bundle bundle) {
        if (bundle != null) {
            AlertDialog alert = new AlertDialog.Builder(MainActivity.this, R.style.DialogTheme)
                .setTitle(R.string.quit_app_alert)
                .setMessage(R.string.quit_app_alert_d)
                .setPositiveButton(R.string.quit_app_alert_yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        Intent intent = new Intent(Intent.ACTION_MAIN);
                        intent.addCategory(Intent.CATEGORY_HOME);
                        startActivity(intent);
                    }
                })
                .setNegativeButton(R.string.quit_app_alert_no, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                    }
                })
                .create();
            alert.getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE, WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE);
            alert.show();
            alert.getWindow().getDecorView().setSystemUiVisibility(getWindow().getDecorView().getSystemUiVisibility());
            alert.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE);
        }
    }

    private void setUpInAppBilling(Bundle bundle) {
        if (bundle != null) {
            mGooglePlayStore = new GooglePlayStore(this);
        }
    }

    private void scheduleNotifications(long []delays, String []titles, short []ids) {
        Context context = getApplicationContext();
        if (delays != null && ids != null && titles != null && ids.length == titles.length && ids.length == delays.length && delays.length > 0) {
            Intent intent = new Intent(context, shnips.MainActivity.AlarmReceiver.class)
                .putExtra(NOTIFICATION_DELAY, delays)
                .putExtra(NOTIFICATION_TITLE, titles)
                .putExtra(NOTIFICATION_ID, ids);

            PendingIntent alarmIntent = PendingIntent.getBroadcast(context, 0, intent, FLAG_CANCEL_CURRENT);
            AlarmManager manager = (AlarmManager)context.getSystemService(Context.ALARM_SERVICE);
            manager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, delays[0], alarmIntent);
        }
    }

    private void setUpWindow() {
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_FULLSCREEN |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
        );
    }
}
