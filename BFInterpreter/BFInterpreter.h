#include <cstdint>
#pragma once
class BFInterpreter {
public:
	enum class Result;
	virtual ~BFInterpreter();
	BFInterpreter(uint64_t dataMemorySize, uint64_t textMemorySize);

	Result Run();
	Result Step();
	void Reset();

	uint64_t GetDataMemorySize() const;
	const char* GetDataBegin() const;
	const char* GetDataPtr() const;
	const char* GetDataEnd() const;

	uint64_t GetTextMemorySize() const;
	const char* GetTextBegin() const;
	const char* GetTextPtr() const;
	const char* GetTextEnd() const;

	uint64_t GetStepCount() const;

	void SetData(uint64_t offset, const char* source, uint64_t length);
	void SetText(uint64_t offset, const char* source, uint64_t length);

	enum class Result {
		Success,				// Step executed successfully
		StepDataBegin,			// Step would result in data pointer going before start of data memory
		StepDataEnd,			// Step would result in data pointer going past end of data memory
		StepTextBegin,			// Step would result in text pointer going before start of text memory
		StepTextEnd,			// Step would result in text pointer going past end of text memory
		StepInvalidInstruction,	// Step would try to execute invalid instruction
	};
private:
	void ExpandData(uint64_t min);
	void ExpandText(uint64_t min);

	inline Result NextData();
	inline Result PrevData();
	inline Result IncData();
	inline Result DecData();
	inline Result PrintData();
	inline Result ReadData();
	inline Result Skip();
	inline Result Rewind();
	char
		* m_dataBegin,
		* m_dataPtr,
		* m_dataEnd;
	uint64_t
		m_dataBytesize;

	char
		* m_textBegin,
		* m_textPtr,
		* m_textEnd;
	uint64_t
		m_textBytesize;

	uint64_t
		m_stepCtr;

	char m_inputBuf[81] = { '\0' };
	char* m_inputPtr;
};