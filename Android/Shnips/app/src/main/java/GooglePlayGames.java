// Copyright © 2018 Igor Pener. All rights reserved.

package shnips;

import android.app.Activity;
import android.content.IntentSender;
import android.nfc.Tag;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.Log;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.achievement.Achievement;
import com.google.android.gms.games.achievement.AchievementBuffer;
import com.google.android.gms.games.achievement.Achievements;
import com.google.android.gms.games.leaderboard.LeaderboardScore;
import com.google.android.gms.games.leaderboard.LeaderboardVariant;
import com.google.android.gms.games.leaderboard.Leaderboards;

import java.util.HashMap;

import static com.google.android.gms.games.GamesActivityResultCodes.RESULT_RECONNECT_REQUIRED;
import static com.google.android.gms.games.GamesActivityResultCodes.RESULT_SIGN_IN_FAILED;
import static com.google.android.gms.games.leaderboard.LeaderboardScore.LEADERBOARD_RANK_UNKNOWN;
import static com.google.android.gms.games.leaderboard.LeaderboardVariant.TIME_SPAN_ALL_TIME;
import static com.google.android.gms.games.leaderboard.LeaderboardVariant.TIME_SPAN_DAILY;
import static com.google.android.gms.games.leaderboard.LeaderboardVariant.TIME_SPAN_WEEKLY;

public class GooglePlayGames implements GoogleApiClient.ConnectionCallbacks, GoogleApiClient.OnConnectionFailedListener {
    private static final int REQUEST_ACHIEVEMENTS_UNUSED = 9003;
    private static final int REQUEST_LEADERBOARD_UNUSED = 9004;
    private static final int REQUEST_SIGN_IN = 1112;

    private static final String TAG = "GooglePlayGames";

    private final String LEADERBOARD_ID = "CgkIxcq8zbsIEAIQAQ";
    private final String[] ACHIEVEMENT_IDS = {
        "CgkIxcq8zbsIEAIQAg",
        "CgkIxcq8zbsIEAIQAw",
        "CgkIxcq8zbsIEAIQBA",
        "CgkIxcq8zbsIEAIQBQ",
        "CgkIxcq8zbsIEAIQBg",
        "CgkIxcq8zbsIEAIQBw",
        "CgkIxcq8zbsIEAIQCA",
        "CgkIxcq8zbsIEAIQCQ",
        "CgkIxcq8zbsIEAIQCg",
        "CgkIxcq8zbsIEAIQCw",
        "CgkIxcq8zbsIEAIQDA",
        "CgkIxcq8zbsIEAIQDQ",
        "CgkIxcq8zbsIEAIQDg",
        "CgkIxcq8zbsIEAIQDw",
        "CgkIxcq8zbsIEAIQEA",
        "CgkIxcq8zbsIEAIQEQ",
        "CgkIxcq8zbsIEAIQEg",
        "CgkIxcq8zbsIEAIQEw",
        "CgkIxcq8zbsIEAIQFA",
        "CgkIxcq8zbsIEAIQFQ",
        "CgkIxcq8zbsIEAIQFg",
        "CgkIxcq8zbsIEAIQFw",
        "CgkIxcq8zbsIEAIQGA",
        "CgkIxcq8zbsIEAIQGQ",
        "CgkIxcq8zbsIEAIQGg",
        "CgkIxcq8zbsIEAIQGw"
    };

    private GoogleApiClient mGoogleApiClient;
    private Activity mActivity;
    private long mScore = 0;
    private Boolean mLeaderboardTapped = false;

    public GooglePlayGames(Activity activity) {
        mActivity = activity;
        mGoogleApiClient = new GoogleApiClient.Builder(activity)
            .addConnectionCallbacks(this)
            .addOnConnectionFailedListener(this)
            .addApi(Games.API).addScope(Games.SCOPE_GAMES)
            .build();
    }

    @Override
    public synchronized void onConnected(@Nullable Bundle bundle) {
        if (mLeaderboardTapped) {
            if (mScore > 0) {
                reportScore(mScore);
            }
            MainActivity.queueEvent(new Runnable() {
                @Override
                public void run() {
                    MainActivity.setAuthenticated(true);
                }
            });
        }
        mLeaderboardTapped = false;
    }

    @Override
    public void onConnectionSuspended(int i) {
        if (mGoogleApiClient != null) {
            mGoogleApiClient.connect();
        }
    }

    @Override
    public void onConnectionFailed(@NonNull ConnectionResult connectionResult) {
        int error = connectionResult.getErrorCode();
        if (error != ConnectionResult.SUCCESS) {
            Log.i(TAG, "onConnectionFailed encountered error: " + connectionResult.getErrorMessage());
        }
        if (connectionResult.hasResolution() && error == ConnectionResult.SIGN_IN_REQUIRED) {
            if (mLeaderboardTapped) {
                try {
                    connectionResult.startResolutionForResult(mActivity, REQUEST_SIGN_IN);
                } catch (IntentSender.SendIntentException e) {
                    Log.e(TAG, "Failed to resolve Google Play Games sign-in: " + e.getMessage());
                }
            }
        } else if (
            error == ConnectionResult.SERVICE_MISSING ||
            error == ConnectionResult.SERVICE_VERSION_UPDATE_REQUIRED ||
            error == ConnectionResult.SERVICE_DISABLED
        ) {
            GoogleApiAvailability googleAPI = GoogleApiAvailability.getInstance();
            int result = googleAPI.isGooglePlayServicesAvailable(mActivity);
            if (result != ConnectionResult.SUCCESS) {
                if (googleAPI.isUserResolvableError(result)) {
                    googleAPI.getErrorDialog(mActivity, result, REQUEST_SIGN_IN).show();
                }
            } else {
                Log.i(TAG, "Unable to connect to Google Play Games. Result: " + result);
            }
        }
    }

    public void showAchievements() {
        if (mGoogleApiClient == null)
            return;

        if (mLeaderboardTapped)
            return;

        mLeaderboardTapped = true;
        if (mGoogleApiClient.isConnected()) {
            mActivity.startActivityForResult(Games.Achievements.getAchievementsIntent(mGoogleApiClient), REQUEST_ACHIEVEMENTS_UNUSED);
        } else {
            mGoogleApiClient.connect();
        }
    }

    public void showLeaderboard() {
        if (mGoogleApiClient == null)
            return;

        if (mLeaderboardTapped)
            return;

        mLeaderboardTapped = true;
        if (mGoogleApiClient.isConnected()) {
            mActivity.startActivityForResult(Games.Leaderboards.getLeaderboardIntent(mGoogleApiClient, LEADERBOARD_ID), REQUEST_LEADERBOARD_UNUSED);
        } else {
            mGoogleApiClient.connect();
        }
    }

    public void loadLeaderboardData() {
        loadLeaderboardDataForTimeScope(TIME_SPAN_DAILY);
        loadLeaderboardDataForTimeScope(TIME_SPAN_WEEKLY);
        loadLeaderboardDataForTimeScope(TIME_SPAN_ALL_TIME);
    }

    // Note: progress is assumed to be of length achievement_type_range
    public void setAchievementsProgress(final float[] progress) {
        if (mGoogleApiClient == null)
            return;

        Games.Achievements.load(mGoogleApiClient, false).setResultCallback(new ResultCallback<Achievements.LoadAchievementsResult>() {
            @Override
            public void onResult(@NonNull Achievements.LoadAchievementsResult loadAchievementsResult) {
                AchievementBuffer buffer = loadAchievementsResult.getAchievements();
                if (buffer == null || buffer.getCount() != progress.length)
                    return;

                HashMap<String, Achievement> achievements = new HashMap<>(progress.length);
                for (Achievement achievement : buffer) {
                    achievements.put(achievement.getAchievementId(), achievement);
                }
                for (short i = 0; i < progress.length; ++i) {
                    if (progress[i] > 1e-5f) {
                        Achievement achievement = achievements.get(ACHIEVEMENT_IDS[i]);

                        if (achievement.getState() == Achievement.STATE_UNLOCKED)
                            continue;

                        if (achievement.getType() == Achievement.TYPE_INCREMENTAL) {
                            final int totalSteps = achievement.getTotalSteps();
                            final int currentSteps = (int)(progress[i] * totalSteps);

                            if (currentSteps > achievement.getCurrentSteps()) {
                                Games.Achievements.setSteps(mGoogleApiClient, ACHIEVEMENT_IDS[i], currentSteps);
                            }
                        } else if (Math.abs(progress[i] - 1.f) < 1e-5f) {
                            Games.Achievements.unlock(mGoogleApiClient, ACHIEVEMENT_IDS[i]);
                        }
                    }
                }
            }
        });
    }

    public void reportScore(long score) {
        if (mGoogleApiClient != null && mGoogleApiClient.isConnected()) {
            mScore = 0;
            Games.Leaderboards.submitScore(mGoogleApiClient, LEADERBOARD_ID, score);
        } else {
            mScore = score;
        }
    }

    public Boolean authenticated() {
        return mGoogleApiClient != null && mGoogleApiClient.isConnected();
    }

    public void onStart() {
        if (mGoogleApiClient != null) {
            mGoogleApiClient.connect();
        }
    }

    public void onStop() {
        if (mGoogleApiClient != null && mGoogleApiClient.isConnected()) {
            mGoogleApiClient.disconnect();
        }
    }

    public void onActivityResult(int requestCode, int resultCode) {
        if (requestCode == REQUEST_LEADERBOARD_UNUSED) {
            mLeaderboardTapped = false;
            if (mGoogleApiClient != null && (resultCode == RESULT_RECONNECT_REQUIRED || resultCode == RESULT_SIGN_IN_FAILED)) {
                mGoogleApiClient.disconnect();
            }
        } else if (requestCode == REQUEST_ACHIEVEMENTS_UNUSED) {
            mLeaderboardTapped = false;
            if (mGoogleApiClient != null && (resultCode == RESULT_RECONNECT_REQUIRED || resultCode == RESULT_SIGN_IN_FAILED)) {
                mGoogleApiClient.disconnect();
            }
        } else if (requestCode == REQUEST_SIGN_IN) {
            if (resultCode == Activity.RESULT_OK) {
                if (mGoogleApiClient != null) {
                    mGoogleApiClient.connect();
                }
            } else {
                mLeaderboardTapped = false;
                if (mGoogleApiClient != null) {
                    mGoogleApiClient.disconnect();
                }
            }
        }
    }

    private void loadLeaderboardDataForTimeScope(final int span) {
        if (mGoogleApiClient == null)
            return;

        Games.Leaderboards.loadCurrentPlayerLeaderboardScore(mGoogleApiClient, LEADERBOARD_ID, span, LeaderboardVariant.COLLECTION_PUBLIC).setResultCallback(
            new ResultCallback<Leaderboards.LoadPlayerScoreResult>() {
                @Override
                public void onResult(@NonNull Leaderboards.LoadPlayerScoreResult loadPlayerScoreResult) {
                    LeaderboardScore score = loadPlayerScoreResult.getScore();
                    if (score == null)
                        return;

                    final long rank = score.getRank() == LEADERBOARD_RANK_UNKNOWN ? 0 : score.getRank();
                    MainActivity.queueEvent(new Runnable() {
                        @Override
                        public void run() {
                            MainActivity.didRetrieveRank(rank, (short)span);
                        }
                    });
                }
            }
        );
    }
}
