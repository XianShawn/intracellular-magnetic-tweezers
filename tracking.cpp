std::vector<cv::Point2f> trackBead()
{
	std::vector<cv::Point2f> resVec;

	cv::Mat blurImg, binImg, sdImg;
	//cv::Size roiSize(100, 100);	

	cv::GaussianBlur(copyImg, blurImg, cv::Size(5,5), 0);

	maxEntropieThreshold(blurImg, binImg);




	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(binImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cv::Point2f tempPt;

	for (int ii = 0; ii < contours.size(); ++ii)
	{
		cv::Rect recBound = cv::boundingRect(contours[ii]);
		if ( recBound.width>=5 && recBound.height>=5 )
		{
			cv::Moments mu = cv::moments(contours[ii], true);
			tempPt.x = mu.m10/mu.m00;
			tempPt.y = mu.m01/mu.m00;
			resVec.push_back(tempPt);
		}
	}

	return resVec;


}

uchar maxEntropieThreshold(const cv::Mat1b& src8, cv::Mat& dst)
{
	// Histogram
	cv::Mat hist;//(1, 256, 0.0);
/*	for (int r=0; r<src8.rows; ++r)
		for (int c=0; c<src8.cols; ++c)
			hist(src8(r,c))++;
*/
	const int histSize[] = {256};  
	float range[] = {0, 255};  
	const float* ranges[] = {range};  
	const int channels[] = {0};  

	cv::calcHist(&src8,1,channels,cv::Mat(),hist,1,histSize,ranges,true,false);  

	// Normalize
	hist /= double(src8.rows * src8.cols);

	// Cumulative histogram
	cv::Mat1d cumhist(1, 256, 0.0);
	float sum = 0;
	for (int i = 0; i < 256; ++i)
	{
		sum += hist.at<float>(i);
		cumhist(i) = sum;
	}

	cv::Mat1d hl(1, 256, 0.0);
	cv::Mat1d hh(1, 256, 0.0);

	for (int t = 0; t < 256; ++t)
	{
		// low range entropy
		double cl = cumhist(t);
		if (cl > 0)
		{
			for (int i = 0; i <= t; ++i)
			{
				if (hist.at<float>(i) > 0)
				{
					hl(t) = hl(t) - (hist.at<float>(i) / cl) * log(hist.at<float>(i) / cl);
				}
			}

		}

		// high range entropy
		double ch = 1.0 - cl;  // constraint cl + ch = 1
		if (ch > 0)
		{
			for (int i = t+1; i < 256; ++i)
			{
				if (hist.at<float>(i) > 0)
				{
					hh(t) = hh(t) - (hist.at<float>(i) / ch) * log(hist.at<float>(i) / ch);
				}
			}
		}
	}

	// choose best threshold

	cv::Mat1d entropie(1, 256, 0.0);
	double h_max = hl(0) + hh(0);
	uchar threshold = 0;
	entropie(0) = h_max;

	for (int t = 1; t < 256; ++t)
	{
		entropie(t) = hl(t) + hh(t);
		if (entropie(t) > h_max)
		{
			h_max = entropie(t);
			threshold = uchar(t);
		}
	}

	// Create output image
	dst = src8 < threshold;

	// Ensure white object on black background
	cv::Scalar meanVal = cv::mean(dst);
	if (meanVal[0] > 120)
	{
		cv::bitwise_not(dst, dst);
	}

	return threshold;
}