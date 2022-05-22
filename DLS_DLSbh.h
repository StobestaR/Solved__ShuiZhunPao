//�ֶ��������죨�Ҷȱ任�ࣩ
//������
//matInput������ͼ��
//matOutput : ���ͼ��
//fStart : �ֶ��������
//fEnd �� �ֶ������յ�
//fSout��ӳ���������
//fEout��ӳ�������յ�
//ע��**��ͨ��8λ�Ҷ�ͼ��**
void dividedLinearStrength(cv::Mat& matInput, cv::Mat& matOutput, float fStart, float fEnd, float fSout, float fEout) {
	//����ֱ�߲���

	//L1��һ�ϵ㲿��б�ʼ��㣬����ϵ��ԭ�㣬����б��
	float fK1 = fSout / fStart;

	//L2�ڶ��ϵ㲿��б�ʼ��㣬����ԭ�㣬����б�ʣ�������˵�ӳ��ֵ
	float fK2 = (fEout - fSout) / (fEnd - fStart);
	float fC2 = fSout - fK2 * fStart;

	//L3�����ϵ㲿��б�ʼ��㣬����ԭ�㣬����б�ʣ�������˵�ӳ��ֵ
	float fK3 = (255.0f - fEout) / (255.0f - fEnd);
	float fC3 = 255.0f - fK3 * 255.0f;

	//������ѯ��
	std::vector<unsigned char> loolUpTable(256);
	for (size_t m = 0; m < 256; m++) {
		if (m < fStart)
			loolUpTable[m] = static_cast<unsigned char>(m * fK1);  //��һ��ӳ��任
		else if (m > fEnd)
			loolUpTable[m] = static_cast<unsigned char>(m * fK3 + fC3);  //�ڶ���ӳ��任
		else
			loolUpTable[m] = static_cast<unsigned char>(m * fK2 + fC2);  //������ӳ��任
	}

	//��ʼ�������С
	matOutput = cv::Mat::zeros(matInput.rows, matInput.cols, matInput.type());

	//�Ҷ�ӳ��
	for (size_t r = 0; r < matInput.rows; r++) {
		unsigned char* pInput = matInput.data + r * matInput.step[0];
		unsigned char* pOutput = matOutput.data + r * matOutput.step[0];
		for (size_t c = 0; c < matInput.cols; c++) {
			//���Gamma�任
			pOutput[c] = loolUpTable[pInput[c]];
		}
	}
}

//����ֱ��ͼ�ķֶ��������죨�Ҷȱ任�ࣩ
//������
//matInput������ͼ��
//matOutput : ���ͼ��
//fH : �߻Ҷȵȼ�����
//fL �� �ͻҶȵȼ�����
//fSigma������ϵ��
//ע��**��ͨ��8λ�Ҷ�ͼ��**
void dlsBaseHistogram(cv::Mat& matInput, cv::Mat& matOutput, float fH, float fL, float fSigma) {
	//ͳ��ֱ��ͼ
	std::vector<int> histogram(256);
	for (size_t r = 0; r < matInput.rows; r++)
		for (size_t c = 0; c < matInput.cols; c++)
			histogram[matInput.at<unsigned char>(r, c)]++;

	//����ֶ�����
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

	//����ӳ������
	float fSout = fStart * fSigma;
	float fEout = fEnd * (fSigma + 1.0f);
	fEout = fEout > 255.0f ? 254 : fEout;

	//�ֶ���������
	dividedLinearStrength(matInput, matOutput, fStart, fEnd, fSout, fEout);
}
