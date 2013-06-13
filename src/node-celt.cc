
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include "../deps/celt-0.7.1/libcelt/celt.h"
#include "common.h"

#include <string.h>

using namespace node;
using namespace v8;

class CeltEncoder : public ObjectWrap {
	private:
		CELTMode* mode;
		CELTEncoder* encoder;
		CELTDecoder* decoder;
		Persistent<Object> modeReference;
		unsigned char compressedBuffer[43];
		celt_int16* frameBuffer;
		int frameSize;

	protected:
		void EnsureEncoder() {
			if( encoder != NULL ) return;
			encoder = celt_encoder_create( mode, 1, NULL );
		}
		void EnsureDecoder() {
			if( decoder != NULL ) return;
			decoder = celt_decoder_create( mode, 1, NULL );
		}

	public:
	   	CeltEncoder( celt_int32 rate, int frame_size ):
			encoder( NULL ), decoder( NULL ), frameSize( frame_size ) {

			mode = celt_mode_create( rate, frame_size, NULL );
			frameBuffer = new celt_int16[frame_size];
		}

		~CeltEncoder() {
			if( encoder != NULL )
				celt_encoder_destroy( encoder );
			if( decoder != NULL )
				celt_decoder_destroy( decoder );

			encoder = NULL;
			decoder = NULL;

			celt_mode_destroy( mode );
			mode = 0;

			delete frameBuffer;
			frameBuffer = 0;
		}

		static Handle<Value> Encode( const Arguments& args ) {
			HandleScope scope;

			REQ_OBJ_ARG( 0, pcmBuffer );
			OPT_INT_ARG( 1, compressedSize, 43 );

			// Read the PCM data.
			char* pcmData = Buffer::Data(pcmBuffer);
			celt_int16* pcm = reinterpret_cast<celt_int16*>( pcmData );

			// Unwrap the encoder.
			CeltEncoder* self = ObjectWrap::Unwrap<CeltEncoder>( args.This() );
			self->EnsureEncoder();

			// Encode the samples.
			int compressedLength = celt_encode( self->encoder, pcm, NULL, &(self->compressedBuffer[0]), compressedSize );

			// Create a new result buffer.
			Buffer* slowBuffer = Buffer::New( compressedLength );
			memcpy(Buffer::Data(slowBuffer), self->compressedBuffer, compressedLength);

			// Wrap the native Buffer into a Node JS buffer.
			Local<Object> globalObj = Context::GetCurrent()->Global();
			Local<Function> ctor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
			Handle<Value> ctorArgs[3] = { slowBuffer->handle_, Integer::New(compressedLength), Integer::New(0) };
			Local<Object> actualBuffer = ctor->NewInstance( 3, ctorArgs );

			return scope.Close( actualBuffer );
		}

		static Handle<Value> Decode( const Arguments& args ) {
			HandleScope scope;

			REQ_OBJ_ARG( 0, compressedBuffer );

			// Read the compressed data.
			unsigned char* compressedData = (unsigned char*)Buffer::Data(compressedBuffer);
			size_t compressedDataLength = Buffer::Length(compressedBuffer);

			CeltEncoder* self = ObjectWrap::Unwrap<CeltEncoder>( args.This() );
			self->EnsureDecoder();

			// Encode the samples.
			celt_decode(
					self->decoder,
					compressedData,
					compressedDataLength,
					&(self->frameBuffer[0]) );

			// Create a new result buffer.
			int dataSize = self->frameSize * 2;
			Buffer* slowBuffer = Buffer::New( dataSize );
			memcpy( Buffer::Data(slowBuffer), reinterpret_cast<char*>(self->frameBuffer), dataSize );

			// Wrap the native Buffer into a Node JS buffer.
			Local<Object> globalObj = Context::GetCurrent()->Global();
			Local<Function> ctor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
			Handle<Value> ctorArgs[3] = { slowBuffer->handle_, Integer::New( dataSize ), Integer::New(0) };
			Local<Object> actualBuffer = ctor->NewInstance( 3, ctorArgs );

			return scope.Close( actualBuffer );
		}

		static Handle<Value> New(const Arguments& args) {
			HandleScope scope;

			if( !args.IsConstructCall()) {
				return ThrowException(Exception::TypeError(
							String::New("Use the new operator to construct the CeltEncoder.")));
			}

			OPT_INT_ARG(0, rate, 42000);
			OPT_INT_ARG(1, size, rate/100);

			CeltEncoder* encoder = new CeltEncoder( rate, size );

			encoder->Wrap( args.This() );
			return args.This();
		}

		static void Init(Handle<Object> exports) {
			Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
			tpl->SetClassName(String::NewSymbol("CeltEncoder"));
			tpl->InstanceTemplate()->SetInternalFieldCount(1);

			tpl->PrototypeTemplate()->Set( String::NewSymbol("encode"),
					FunctionTemplate::New( Encode )->GetFunction() );

			tpl->PrototypeTemplate()->Set( String::NewSymbol("decode"),
					FunctionTemplate::New( Decode )->GetFunction() );

			Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
			exports->Set(String::NewSymbol("CeltEncoder"), constructor);
		}
};


void init(Handle<Object> exports) {
	CeltEncoder::Init( exports );
}

NODE_MODULE(node_celt, Init)

