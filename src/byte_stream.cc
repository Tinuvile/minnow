#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : buffer_()
  , start_position_( 0 )
  , bytes_pushed_( 0 )
  , bytes_popped_( 0 )
  , closed_( false )
  , eof_( false )
  , capacity_( capacity )
  , error_( false )
{}

void Writer::push( string data )
{
  if ( closed_ || error_ )
    return;

  const uint64_t available = available_capacity();
  const uint64_t byte_to_push = min( data.size(), available );

  if ( byte_to_push > 0 ) {
    buffer_.append( data.substr( 0, byte_to_push ) );
    bytes_pushed_ += byte_to_push;
  }
}

void Writer::close()
{
  closed_ = true;
}

bool Writer::is_closed() const
{
  return closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - ( buffer_.size() - start_position_ );
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  return string_view( buffer_ ).substr( start_position_ );
}

void Reader::pop( uint64_t len )
{
  len = min( len, buffer_.size() - start_position_ );
  start_position_ += len;
  bytes_popped_ += len;

  // if data_read is greater than 4 KB and half of the buffer is used,
  // remove the unused part of the buffer
  if ( start_position_ > 4096 && start_position_ >= buffer_.size() / 2 ) {
    buffer_ = buffer_.substr( start_position_ );
    start_position_ = 0;
  }
}

bool Reader::is_finished() const
{
  return closed_ && ( start_position_ == buffer_.size() );
}

uint64_t Reader::bytes_buffered() const
{
  return buffer_.size() - start_position_;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}
