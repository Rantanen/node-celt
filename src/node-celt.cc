
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>
#include "../deps/celt-0.7.1/libcelt/celt.h"
#include "common.h"
#include <nan.h>

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

		static NAN_METHOD(Encode) {
			NanScope();

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
			Local<Object> actualBuffer = NanNewBufferHandle(reinterpret_cast<char*>(self->compressedBuffer), compressedLength );


			NanReturnValue( actualBuffer );
		}

		static NAN_METHOD(Decode) {
			NanScope();

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
			Local<Object> actualBuffer = NanNewBufferHandle(reinterpret_cast<char*>(self->frameBuffer), dataSize);


			NanReturnValue( actualBuffer );
		}

		static NAN_METHOD(New) {
			NanScope();

			if( !args.IsConstructCall()) {
				NanThrowTypeError("Use the new operator to construct the CeltEncoder.");
				NanReturnUndefined();
			}

			OPT_INT_ARG(0, rate, 42000);
			OPT_INT_ARG(1, size, rate/100);

			CeltEncoder* encoder = new CeltEncoder( rate, size );

			encoder->Wrap( args.This() );
			NanReturnValue(args.This());
		}

		static void Init(Handle<Object> exports) {
			Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
			tpl->SetClassName(NanNew<String>("CeltEncoder"));
			tpl->InstanceTemplate()->SetInternalFieldCount(1);

			tpl->PrototypeTemplate()->Set( NanNew<String>("encode"),
				NanNew<FunctionTemplate>( Encode )->GetFunction() );

			tpl->PrototypeTemplate()->Set( NanNew<String>("decode"),
				NanNew<FunctionTemplate>( Decode )->GetFunction() );

			//v8::Persistent<v8::FunctionTemplate> constructor;
			//NanAssignPersistent(constructor, tpl);
			exports->Set(NanNew<String>("CeltEncoder"), tpl->GetFunction());
		}
};


void NodeInit(Handle<Object> exports) {
	CeltEncoder::Init( exports );
}

NODE_MODULE(node_celt, NodeInit)
