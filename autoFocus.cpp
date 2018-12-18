void Autofocus(double search_range, double stepSearchDirection, cv::Rect search_ROI)
{
	clock_t m_time_start = clock();

	//	First determine the search direction
	double dIniZ = m_prior.Focus().GetPosition();

	const double dStepSearchDirection = stepSearchDirection;	//20

	m_cs_copy_mat.Lock();
	double dIniFocus = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();
	std::cout<<"z: "<<m_prior.Focus().GetPosition()<<",   dIniFocus:  "<<dIniFocus<<std::endl;

	m_prior.Focus().MoveUp(dStepSearchDirection);
	m_prior.WaitForFocusStop();
	m_cs_copy_mat.Lock();
	double dUpFocus = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();
	std::cout<<"z: "<<m_prior.Focus().GetPosition()<<",   dUpFocus:  "<<dUpFocus<<std::endl;

	m_prior.Focus().MoveDown(2.0*dStepSearchDirection);
	m_prior.WaitForFocusStop();
	m_cs_copy_mat.Lock();
	double dDownFocus = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();
	std::cout<<"z: "<<m_prior.Focus().GetPosition()<<",   dDownFocus:  "<<dDownFocus<<std::endl;

	m_prior.Focus().MoveToAbsolute(dIniZ);
	m_prior.WaitForFocusStop();

	if (dIniFocus > dUpFocus && dIniFocus > dDownFocus)
		return;

	bool bIsMovingUp = dUpFocus > dDownFocus ? true : false;
	std::cout<<"is Moving Up? "<<bIsMovingUp<<std::endl;

	//	Calculate next Fibonacci search position (search space is const 900 um)
	int N = 0;
	const double Fib[23]  = {0.,1.,1.,2.,3.,5.,8.,13.,21.,34.,55.,89.,144.,233.,377.,610.,987.,1597.,2584.,4181.,6765.,10946.,17711.};
	double a = 0.;
	double b = search_range > 0 ? search_range : -search_range;
	double x1, x2, y1, y2, dMoveStep;

	//	Determine the minimum N such that Fib(N) >= search_range;
	for (int ii = 0; ii < 23; ii++)
	{
		if (Fib[ii] > search_range)
		{
			N = ii;
			std::cout<<"Fibonacci N: "<<N<<std::endl;
			break;
		}
	}


	x1 = a + Fib[N-2]*(b-a)/Fib[N];
	x2 = b - Fib[N-2]*(b-a)/Fib[N];

	dMoveStep = bIsMovingUp ? x1 : -x1;	// moving up, motor z position gets smaller
	m_prior.Focus().MoveToAbsolute(dIniZ + dMoveStep);
	m_prior.WaitForFocusStop();
	m_cs_copy_mat.Lock();
	y1 = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();

	std::cout<<"step: "<<dMoveStep<<" position: "<< m_prior.Focus().GetPosition()<< ",  focus: "<<y1<<std::endl;

	dMoveStep = bIsMovingUp ? x2 : -x2;
	m_prior.Focus().MoveToAbsolute(dIniZ + dMoveStep);
	m_prior.WaitForFocusStop();
	m_cs_copy_mat.Lock();
	y2 = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();

	std::cout<<"step: "<<dMoveStep<<" position: "<< m_prior.Focus().GetPosition()<< ",  focus: "<<y2<<std::endl;

	//	iterate until the best focus is found
	for (int ii = N - 1; ii > 2; ii--)
	{
		if  (y1 < y2)
		{
			a = x1;
			x1 = x2;
			y1 = y2;
			x2 = b - Fib[ii-2]*(b-a)/Fib[ii];

			dMoveStep = bIsMovingUp ? x2 : -x2;
			m_prior.Focus().MoveToAbsolute(dIniZ + dMoveStep);
			m_prior.WaitForFocusStop();
			m_cs_copy_mat.Lock();
			y2 = Methods::GetFocusScore(m_copyImg, search_ROI);
			m_cs_copy_mat.Unlock();


			std::cout<<"step: "<<dMoveStep<<" position: "<< m_prior.Focus().GetPosition()<< ",  focus: "<<y2<<std::endl;
		}
		else
		{
			b = x2;
			x2 = x1;
			y2 = y1;
			x1 = a + Fib[ii-2]*(b-a)/Fib[ii];

			dMoveStep = bIsMovingUp ? x1 : -x1;
			m_prior.Focus().MoveToAbsolute(dIniZ + dMoveStep);
			m_prior.WaitForFocusStop();
			m_cs_copy_mat.Lock();
			y1 = Methods::GetFocusScore(m_copyImg, search_ROI);
			m_cs_copy_mat.Unlock();

			std::cout<<"step: "<<dMoveStep<<" position: "<< m_prior.Focus().GetPosition()<< ",  focus: "<<y1<<std::endl;
		}
	}

	clock_t m_time_stop = clock();
	std::cout<<"Total time consumed: "<<(double)(m_time_stop-m_time_start)/(double)CLOCKS_PER_SEC<<" s"<<std::endl;
	std::cout<<"Autofocusing completed!"<<std::endl;
}

double GetFocusScore(cv::Mat _scrImg, cv::Rect ROI)
{
	double focus_measure = 0.0;
	cv::Scalar scalarMean; 

	cv::Mat copyImg;

	if (ROI.x==0 && ROI.y==0)
	{
		_scrImg.copyTo(copyImg);
	}
	else
	{
		_scrImg(ROI).copyTo(copyImg);
	}
	cv::Mat sobelMat_x, sobelMat_y, sobelMat_pow;   // CV_32FC1  CV_16SC1
	cv::Sobel(_scrImg, sobelMat_x, CV_32FC1 , 1, 0, 3);   //[-1 0 1; -2 0 2; -1 0 1]
	cv::Sobel(_scrImg, sobelMat_y, CV_32FC1 , 0, 1, 3);  //cv::BORDER_DEFAULT is the different from the old getFocusScore
	cv::pow( sobelMat_x, 2.0, sobelMat_x );
	cv::pow( sobelMat_y, 2.0, sobelMat_y );
	sobelMat_pow = sobelMat_x + sobelMat_y; 
	//cv::add( sobelMat_x, sobelMat_y, sobelMat_pow);

	/// method 01 //// //! computes mean value of selected array elements
	scalarMean = cv::mean( sobelMat_pow );
	focus_measure = scalarMean.val[0];

	return focus_measure;

	// 	double focus_measure = 0.0; 
	// 	cv::Scalar scalarMean, scalarStdev; 
	// 	cv::meanStdDev( _scrImg, scalarMean, scalarStdev); 
	// 	//	focus_measure = std::pow( scalarStdev.val[0], 2) / scalarMean.val[0]; 
	// 	focus_measure = scalarStdev.val[0];
	// 	return focus_measure; 

}