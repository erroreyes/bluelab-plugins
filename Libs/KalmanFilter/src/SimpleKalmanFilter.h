/*
 * SimpleKalmanFilter - a Kalman Filter implementation for single variable models.
 * Created by Denys Sene, January, 1, 2017.
 * Released under MIT License - see LICENSE file for details.
 */

#ifndef SimpleKalmanFilter_h
#define SimpleKalmanFilter_h

#include <BLTypes.h>

class SimpleKalmanFilter
{

public:
  SimpleKalmanFilter(BL_FLOAT mea_e, BL_FLOAT est_e, BL_FLOAT q);
    
  // Niko
  // Initialize the first value
  void initEstimate(BL_FLOAT mea);

  BL_FLOAT updateEstimate(BL_FLOAT mea);
  void setMeasurementError(BL_FLOAT mea_e);
  void setEstimateError(BL_FLOAT est_e);
  void setProcessNoise(BL_FLOAT q);
  BL_FLOAT getKalmanGain();
  BL_FLOAT getEstimateError();

private:
  BL_FLOAT _err_measure;
  BL_FLOAT _err_estimate;
  BL_FLOAT _q;
  BL_FLOAT _current_estimate;
  BL_FLOAT _last_estimate;
  BL_FLOAT _kalman_gain;

};

#endif
