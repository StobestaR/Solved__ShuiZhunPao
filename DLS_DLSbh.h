//分段线性拉伸（灰度变换类）
//参数：
//matInput：输入图像
//matOutput : 输出图像
//fStart : 分段区间起点
//fEnd ： 分段区间终点
//fSout：映射区间起点
//fEout：映射区间终点
//注：**单通道8位灰度图像**
void dividedLinearStrength(cv::Mat& matInput, cv::Mat& matOutput, float fStart, float fEnd, float fSout, float fEout) {
	//计算直线参数

	//L1第一断点部分斜率计算，坐标系过原点，计算斜率
	float fK1 = fSout / fStart;

	//L2第二断点部分斜率计算，不过原点，计算斜率，计算左端点映射值
	float fK2 = (fEout - fSout) / (fEnd - fStart);
	float fC2 = fSout - fK2 * fStart;

	//L3第三断点部分斜率计算，不过原点，计算斜率，计算左端点映射值
	float fK3 = (255.0f - fEout) / (255.0f - fEnd);
	float fC3 = 255.0f - fK3 * 255.0f;

	//建立查询表
	std::vector<unsigned char> loolUpTable(256);
	for (size_t m = 0; m < 256; m++) {
		if (m < fStart)
			loolUpTable[m] = static_cast<unsigned char>(m * fK1);  //第一段映射变换
		else if (m > fEnd)
			loolUpTable[m] = static_cast<unsigned char>(m * fK3 + fC3);  //第二段映射变换
		else
			loolUpTable[m] = static_cast<unsigned char>(m * fK2 + fC2);  //第三段映射变换
	}

	//初始化输出大小
	matOutput = cv::Mat::zeros(matInput.rows, matInput.cols, matInput.type());

	//灰度映射
	for (size_t r = 0; r < matInput.rows; r++) {
		unsigned char* pInput = matInput.data + r * matInput.step[0];
		unsigned char* pOutput = matOutput.data + r * matOutput.step[0];
		for (size_t c = 0; c < matInput.cols; c++) {
			//查表Gamma变换
			pOutput[c] = loolUpTable[pInput[c]];
		}
	}
}

//基于直方图的分段线性拉伸（灰度变换类）
//参数：
//matInput：输入图像
//matOutput : 输出图像
//fH : 高灰度等级比例
//fL ： 低灰度等级比例
//fSigma：拉伸系数
//注：**单通道8位灰度图像**
void dlsBaseHistogram(cv::Mat& matInput, cv::Mat& matOutput, float fH, float fL, float fSigma) {
	//统计直方图
	std::vector<int> histogram(256);
	for (size_t r = 0; r < matInput.rows; r++)
		for (size_t c = 0; c < matInput.cols; c++)
			histogram[matInput.at<unsigned char>(r, c)]++;

	//计算分段区间
	int nNH = matInput.rows * matInput.cols * fH;
	int nNL = matInput.rows * matInput.cols * fL;
	int nACC = 0;
	float fStart = 0, fEnd = 0;

	for (size_t m = 255; m >= 0; m--) {
		nACC += histogram[m];
		if (nACC > nNH) {
			fEnd = m;
			break;
		}
	}

	nACC = 0;
	for (size_t m = 0; m < histogram.size(); m++) {
		nACC += histogram[m];
		if (nACC > nNL) {
			fStart = m;
			break;
		}
	}

	//计算映射区间
	float fSout = fStart * fSigma;
	float fEout = fEnd * (fSigma + 1.0f);
	fEout = fEout > 255.0f ? 254 : fEout;

	//分段线性拉伸
	dividedLinearStrength(matInput, matOutput, fStart, fEnd, fSout, fEout);
}
