#include "BFInterpreter.h"
#include <stdio.h>
BFInterpreter::~BFInterpreter() {}
BFInterpreter::BFInterpreter(uint64_t dataMemorySize, uint64_t textMemorySize) :
	m_stepCtr(0) {
	m_dataBytesize = dataMemorySize;
	m_textBytesize = textMemorySize;

	m_dataBegin = m_dataPtr = new char[dataMemorySize];
	m_dataEnd = m_dataBegin + m_dataBytesize;
	for (auto* it = m_dataBegin, *jt = m_dataEnd; it != jt; it++)
		*it = 0;

	m_textBegin = m_textPtr = new char[textMemorySize];
	m_textEnd = m_textBegin + m_textBytesize;
	for (auto* it = m_textBegin, *jt = m_textEnd; it != jt; it++)
		*it = 0;

	m_inputPtr = m_inputBuf;
}

BFInterpreter::Result BFInterpreter::Run() {
	Result res = Result::Success;
	while ((res = Step()) == Result::Success && *m_textPtr != '\0');
	return res;
}
BFInterpreter::Result BFInterpreter::Step() {
	Result ret = Result::Success;

	// Execute instruction
	char data = *m_dataPtr;
	char instruction = *m_textPtr;
	switch (instruction) {
	break; case '>':
		ret = NextData();
	break; case '<':
		ret = PrevData();
	break; case '+':
		ret = IncData();
	break; case '-':
		ret = DecData();
	break; case '.':
		ret = PrintData();
	break; case ',':
		ret = ReadData();
	break; case '[':
		if (data == 0) ret = Skip();
	break; case ']':
		if (data != 0) ret = Rewind();
	break; default:
		// DO NOTHING...
		break;
	}
	if (ret != Result::Success) return ret;	// Abort if error

	// Next instruction
	if (m_textPtr == (m_textEnd - 1)) return Result::StepTextEnd;// Abort if error
	m_textPtr++;

	// Update step counter
	m_stepCtr++;
	return ret;
}
void BFInterpreter::Reset() {
	m_dataPtr = m_dataBegin;
	m_textPtr = m_textBegin;
	m_stepCtr = 0;

}

uint64_t BFInterpreter::GetDataMemorySize() const { return m_dataBytesize; }
const char* BFInterpreter::GetDataBegin() const { return m_dataBegin; }
const char* BFInterpreter::GetDataPtr() const { return m_dataPtr; }
const char* BFInterpreter::GetDataEnd() const { return m_dataEnd; }

uint64_t BFInterpreter::GetTextMemorySize() const { return m_textBytesize; }
const char* BFInterpreter::GetTextBegin() const { return m_textBegin; }
const char* BFInterpreter::GetTextPtr() const { return m_textPtr; }
const char* BFInterpreter::GetTextEnd() const { return m_textEnd; }
uint64_t BFInterpreter::GetStepCount() const { return m_stepCtr; }

void BFInterpreter::SetData(uint64_t offset, const char* source, uint64_t length) {
	// Test if expansion is needed
	if (offset + length > m_dataBytesize) ExpandData(offset + length);

	const char* rd = source;
	char* wr = m_dataBegin + offset;
	while (length--)
		*wr++ = *rd++;
}
void BFInterpreter::SetText(uint64_t offset, const char* source, uint64_t length) {
	// Test if expansion is needed
	if (offset + length > m_dataBytesize) ExpandText(offset + length);

	const char* rd = source;
	char* wr = m_textBegin + offset;
	while (length--)
		*wr++ = *rd++;
}
void BFInterpreter::ExpandData(uint64_t sz) {
	// allocate new buffer
	char* newBuffer = new char[sz];
	char* newPtr = newBuffer + (m_dataPtr - m_dataBegin);
	char* newEnd = newBuffer + sz;

	// copy data
	char* dst = newBuffer;
	for (auto* src = m_dataBegin, *dst = newBuffer; src < m_dataEnd; )
		*dst++ = *src++;

	// zero data
	for (; dst != newEnd; dst++) *dst = 0;

	// delete old buffer
	delete m_dataBegin;

	// remap start/ptr/end
	m_dataBegin = newBuffer;
	m_dataPtr = newPtr;
	m_dataEnd = newEnd;
}
void BFInterpreter::ExpandText(uint64_t sz) {
	// allocate new buffer
	char* newBuffer = new char[sz];
	char* newPtr = newBuffer + (m_textPtr - m_textBegin);
	char* newEnd = newBuffer + sz;

	// copy data
	char* dst = newBuffer;
	for (auto* src = m_textBegin, *dst = newBuffer; src < m_textEnd; )
		*dst++ = *src++;

	// zero data
	for (; dst != newEnd; dst++) *dst = 0;

	// delete old buffer
	delete m_textBegin;

	// remap start/ptr/end
	m_textBegin = newBuffer;
	m_textPtr = newPtr;
	m_textEnd = newEnd;
}
BFInterpreter::Result BFInterpreter::NextData() {
	if (m_dataPtr == (m_dataEnd - 1))
		return Result::StepDataEnd;

	m_dataPtr++;
	return Result::Success;
}
BFInterpreter::Result BFInterpreter::PrevData() {
	if (m_dataPtr == m_dataBegin)
		return Result::StepDataBegin;

	m_dataPtr--;
	return Result::Success;
}
BFInterpreter::Result BFInterpreter::IncData() {
	(*m_dataPtr)++;
	return Result::Success;
}
BFInterpreter::Result BFInterpreter::DecData() {
	(*m_dataPtr)--;
	return Result::Success;
}
BFInterpreter::Result BFInterpreter::PrintData() {
	printf_s("%c", *m_dataPtr);
	return Result::Success;
}
BFInterpreter::Result BFInterpreter::ReadData() {
	if (m_inputPtr == m_inputBuf || *m_inputPtr == '\n' || *m_inputPtr == '\0') {
		printf_s("%s", "> ");
		scanf_s("%s", m_inputBuf, static_cast<uint32_t>(sizeof(m_inputBuf) - 1));
		m_inputPtr = m_inputBuf;
	}

	// post data
	*m_dataPtr = *m_inputPtr;
	m_inputPtr++;
	return Result::Success;
}
BFInterpreter::Result BFInterpreter::Skip() {
	uint64_t bracketCtr = 1;
	while (bracketCtr != 0) {
		// Test bounds
		if (m_textPtr == (m_textEnd - 1)) return BFInterpreter::Result::StepTextEnd;

		// Progress pointer
		m_textPtr++;

		// Test bracket balance
		if (*m_textPtr == L'[')bracketCtr++;
		else if (*m_textPtr == L']')bracketCtr--;
	}
	return BFInterpreter::Result::Success;
}
BFInterpreter::Result BFInterpreter::Rewind() {
	uint64_t bracketCtr = 1;
	while (bracketCtr != 0) {
		// Test bounds
		if (m_textPtr == m_textBegin) return BFInterpreter::Result::StepTextBegin;

		// Progress pointer
		m_textPtr--;

		// Test bracket balance
		if (*m_textPtr == L']') bracketCtr++;
		else if (*m_textPtr == L'[') bracketCtr--;
	}
	return BFInterpreter::Result::Success;
}


