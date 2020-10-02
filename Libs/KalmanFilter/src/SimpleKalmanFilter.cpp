/*
 * SimpleKalmanFilter - a Kalman Filter implementation for single variable models.
 * Created by Denys Sene, January, 1, 2017.
 * Released under MIT License - see LICENSE file for details.
 */

// Niko: comment
//#include "Arduino.h"

// Niko: switched from float to BL_FLOAT

#include "SimpleKalmanFilter.h"
#include <math.h>

SimpleKalmanFilter::SimpleKalmanFilter(BL_FLOAT mea_e, BL_FLOAT est_e, BL_FLOAT q)
{
  _err_measure=mea_e;
  _err_estimate=est_e;
  _q = q;
}

// Niko
// Initialize the first value
void SimpleKalmanFilter::initEstimate(BL_FLOAT mea)
{
    //_current_estimate = mea;
    _last_estimate = mea;
}

BL_FLOAT SimpleKalmanFilter::updateEstimate(BL_FLOAT mea)
{
  _kalman_gain = _err_estimate/(_err_estimate + _err_measure);
  _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
  _err_estimate =  (1.0 - _kalman_gain)*_err_estimate + fabs(_last_estimate-_current_estimate)*_q;
  _last_estimate=_current_estimate;

  return _current_estimate;
}

void SimpleKalmanFilter::setMeasurementError(BL_FLOAT mea_e)
{
  _err_measure=mea_e;
}

void SimpleKalmanFilter::setEstimateError(BL_FLOAT est_e)
{
  _err_estimate=est_e;
}

void SimpleKalmanFilter::setProcessNoise(BL_FLOAT q)
{
  _q=q;
}

BL_FLOAT SimpleKalmanFilter::getKalmanGain() {
  return _kalman_gain;
}

BL_FLOAT SimpleKalmanFilter::getEstimateError() {
  return _err_estimate;
}
