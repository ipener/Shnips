// Copyright © 2018 Igor Pener. All rights reserved.

package shnips;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.WindowManager;
import com.android.vending.billing.IInAppBillingService;
import java.util.ArrayList;
import java.util.UUID;
import org.json.JSONException;
import org.json.JSONObject;

public class GooglePlayStore {
    public static final int PURCHASE_REQUEST = 1001;
    private static final int BILLING_RESPONSE_RESULT_OK = 0;
    private static final int BILLING_RESPONSE_RESULT_USER_CANCELED = 1;
    private static final int BILLING_RESPONSE_RESULT_SERVICE_UNAVAILABLE = 2;
    private static final int BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED = 7;

    private static final String TAG = "GooglePlayStore";
    private static final String IN_APP_PURCHASE_ID_HEARTS = "ch.techos.shnips2.in_app_purchase_type_hearts";

    private Activity mActivity;
    private IInAppBillingService mInAppBillingService;
    private ServiceConnection mServiceConnection;
    private String mDeveloperPayload;

    public GooglePlayStore(Activity activity) {
        mActivity = activity;
        mServiceConnection = new ServiceConnection() {
            @Override
            public void onServiceDisconnected(ComponentName name) {
                mInAppBillingService = null;
            }

            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                mInAppBillingService = IInAppBillingService.Stub.asInterface(service);
                requestProducts(mActivity.getPackageName(), mInAppBillingService);
            }
        };
        Intent intent = new Intent("com.android.vending.billing.InAppBillingService.BIND")
            .setPackage("com.android.vending");
        mActivity.bindService(intent, mServiceConnection, Context.BIND_AUTO_CREATE);
    }

    public void startPurchase(short type) {
        if (type != 0) {
            Log.e(TAG, "Requested invalid in app purchase type: " + type);
            MainActivity.queueEvent(new Runnable() {
                @Override
                public void run() {
                    MainActivity.didCancelInAppPurchase();
                }
            });
        } else {
            try {
                final String productId = IN_APP_PURCHASE_ID_HEARTS;
                mDeveloperPayload = UUID.randomUUID().toString();
                Bundle bundle = mInAppBillingService.getBuyIntent(3, mActivity.getPackageName(), productId, "inapp", mDeveloperPayload);
                PendingIntent intent = bundle.getParcelable("BUY_INTENT");
                int responseCode = bundle.getInt("RESPONSE_CODE");
                if (responseCode == BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED) {
                    MainActivity.queueEvent(new Runnable() {
                        @Override
                        public void run() {
                            MainActivity.didBuyInAppPurchase((short)0);
                        }
                    });
                } else {
                    mActivity.startIntentSenderForResult(intent.getIntentSender(), PURCHASE_REQUEST, new Intent(), 0, 0, 0);
                }
            } catch (Exception e) {
                MainActivity.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        MainActivity.didCancelInAppPurchase();
                    }
                });
                Log.e(TAG, "Failed to start purchase. Received exception: " + e.getMessage());

                AlertDialog alert = new AlertDialog.Builder(mActivity, R.style.DialogTheme)
                    .setTitle(R.string.purchase_unavailable)
                    .setMessage(R.string.purchase_unavailable_d)
                    .setCancelable(false)
                    .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {}
                    })
                    .create();
                alert.getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE, WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE);
                alert.show();
                alert.getWindow().getDecorView().setSystemUiVisibility(mActivity.getWindow().getDecorView().getSystemUiVisibility());
                alert.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE);
            }
        }
    }

    private void requestProducts(final String packageName, final IInAppBillingService service) {
        if (service == null)
            return;

        new Thread(new Runnable() {
            public void run() {
                try {
                    ArrayList<String> productIds = new ArrayList<String>();
                    productIds.add(IN_APP_PURCHASE_ID_HEARTS);
                    Bundle bundle = new Bundle();
                    bundle.putStringArrayList("ITEM_ID_LIST", productIds);
                    Bundle details = service.getSkuDetails(3, packageName, "inapp", bundle);
                    int responseCode = details.getInt("RESPONSE_CODE");
                    if (responseCode == BILLING_RESPONSE_RESULT_OK) {
                        ArrayList<String> responseList = details.getStringArrayList("DETAILS_LIST");

                        for (String response : responseList) {
                            try {
                                JSONObject object = new JSONObject(response);
                                final String productId = object.getString("productId");
                                final String price = object.getString("price");

                                if (productId.equals(IN_APP_PURCHASE_ID_HEARTS)) {
                                    MainActivity.queueEvent(new Runnable() {
                                        @Override
                                        public void run() {
                                            MainActivity.didRetrievePurchase(price, (short)0);
                                        }
                                    });
                                } else {
                                    Log.w(TAG, "Unexpected product retrieved:" + productId);
                                }
                            } catch (JSONException e) {
                                Log.e(TAG, "Failed to parse response: " + response + " received exception:" + e.getMessage());
                                return;
                            }
                        }
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    public void onDestroy() {
        if (mInAppBillingService != null) {
            mActivity.unbindService(mServiceConnection);
        }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        final int responseCode = data.getIntExtra("RESPONSE_CODE", 0);
        final String purchaseData = data.getStringExtra("INAPP_PURCHASE_DATA");
        Boolean didCancelInAppPurchase = false;

        if (resultCode == Activity.RESULT_OK) {
            if (responseCode == GooglePlayStore.BILLING_RESPONSE_RESULT_OK) {
                if (mInAppBillingService == null) {
                    didCancelInAppPurchase = true;
                } else {
                    try {
                        JSONObject object = new JSONObject(purchaseData);
                        if (mDeveloperPayload.equals(object.getString("developerPayload"))) {
                            final String productId = object.getString("productId");
                            if (productId.equals(IN_APP_PURCHASE_ID_HEARTS)) {
                                MainActivity.queueEvent(new Runnable() {
                                    @Override
                                    public void run() {
                                        MainActivity.didBuyInAppPurchase((short)0);
                                    }
                                });
                            } else {
                                Log.w(TAG, "Unexpected product found in purchase response: " + productId);
                                didCancelInAppPurchase = true;
                            }
                        } else {
                            Log.e(TAG, "Retrieved unexpected developer payload: " + object.getString("developerPayload"));
                            didCancelInAppPurchase = true;
                        }
                    } catch (JSONException e) {
                        Log.w(TAG, "Failed to consume purchase. Received exception: " + e.getMessage());
                        didCancelInAppPurchase = true;
                    }
                }
            } else if (responseCode == GooglePlayStore.BILLING_RESPONSE_RESULT_USER_CANCELED) {
                Log.i(TAG, "User cancelled in app purchase");
                didCancelInAppPurchase = true;
            } else if (responseCode == GooglePlayStore.BILLING_RESPONSE_RESULT_SERVICE_UNAVAILABLE) {
                Log.i(TAG, "Billing service unavailable");
                AlertDialog alert = new AlertDialog.Builder(mActivity, R.style.DialogTheme)
                    .setTitle(R.string.purchase_cancelled)
                    .setMessage(R.string.cannot_connect)
                    .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                        }
                    })
                    .create();
                alert.getWindow().setFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE, WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE);
                alert.show();
                alert.getWindow().getDecorView().setSystemUiVisibility(mActivity.getWindow().getDecorView().getSystemUiVisibility());
                alert.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE);
                didCancelInAppPurchase = true;
            } else {
                didCancelInAppPurchase = true;
            }
        } else {
            Log.i(TAG, "Billing service API not supported");
            didCancelInAppPurchase = true;
        }
        if (didCancelInAppPurchase) {
            MainActivity.queueEvent(new Runnable() {
                @Override
                public void run() {
                    MainActivity.didCancelInAppPurchase();
                }
            });
        }
    }
}
