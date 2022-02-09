// Copyright © 2018 Igor Pener. All rights reserved.

package shnips;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.support.v4.view.MotionEventCompat;
import android.view.MotionEvent;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLView extends GLSurfaceView {
    public class GLRenderer implements GLSurfaceView.Renderer {
        private long mBeginClock = 0;
        private long mEndClock;
        private float mDensity;

        private GLRenderer(float density) {
            this.mDensity = density;
            start();
        }

        private synchronized void start() {
            mBeginClock = System.nanoTime();
        }

        public synchronized void onDrawFrame(GL10 gl) {
            mEndClock = System.nanoTime();
            MainActivity.update((mEndClock - mBeginClock) / 1e9f);
            mBeginClock = mEndClock;
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            MainActivity.init(width / mDensity, height / mDensity, mDensity);
            mBeginClock = System.nanoTime();
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            MainActivity.dealloc();
        }
    }

    private final GLRenderer mRenderer;

    public GLView(Context context) {
        super(context);
        mRenderer = new GLRenderer(getResources().getDisplayMetrics().density);
        setEGLConfigChooser(8, 8, 8, 8, 0, 0);
        setEGLContextClientVersion(3);
        setPreserveEGLContextOnPause(true);
        setRenderer(mRenderer);
    }

    @Override
    public void setRenderMode(int renderMode) {
        if (renderMode == RENDERMODE_CONTINUOUSLY) {
            mRenderer.start();
        }
        super.setRenderMode(renderMode);
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onResume() {
        mRenderer.start();
        super.onResume();
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        final double x = e.getX() / mRenderer.mDensity;
        final double y = e.getY() / mRenderer.mDensity;
        final long t = e.getEventTime();
        switch(MotionEventCompat.getActionMasked(e)) {
            case MotionEvent.ACTION_DOWN:
                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        MainActivity.touchesBegan(x, y, t);
                    }
                });
                return true;
            case MotionEvent.ACTION_MOVE:
                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        MainActivity.touchesMoved(x, y, t);
                    }
                });
                return true;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_OUTSIDE:
                queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        MainActivity.touchesEnded(x, y, t);
                    }
                });
                return true;
            default :
                return super.onTouchEvent(e);
        }
    }
}
