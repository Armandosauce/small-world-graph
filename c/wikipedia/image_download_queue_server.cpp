// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "ImageDownloadQueue.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include "tuple_image_mapper.h"
#include "thread_safe_queue.h"
#include <google/dense_hash_set>
#include "equality.h"
#include "paul_hsieh_hash.h"
#include "s3_helper.h"
#include <sstream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace std;

using boost::shared_ptr;
using google::dense_hash_set;

#define CHUNK_SIZE 35

/* Globals */
ThreadSafeQueue<ImageWorkUnit> __images;

class ImageDownloadQueueHandler : virtual public ImageDownloadQueueIf {
 public:
  ImageDownloadQueueHandler() {
  }

  void dequeue(std::vector<ImageWorkUnit> & _return) {
    try {
      for(int i=0; i < CHUNK_SIZE; i++) {
        ImageWorkUnit unit =  __images.dequeue();
        _return.push_back(unit);
      }
    } catch (QueueEmptyException& qe) {
    }
    cout << "Dequeued " << _return.size() << " work units. " << __images.size() << " Remain." << endl;
  }

  void enqueue(const std::vector<ImageWorkUnit> & units) {
    for(vector<ImageWorkUnit>::const_iterator ii = units.begin(); ii != units.end(); ii++) {
      __images.enqueue(*ii);
    }
    cout << "Enqueued " << units.size() << " work units. " << __images.size() << " in queue." << endl;
  }

 protected:
  uint32_t desired_size_;
  ThreadSafeQueue<const char*> images_;
};

int main(int argc, char **argv) {
  int port = 9091;
  shared_ptr<ImageDownloadQueueHandler> handler(new ImageDownloadQueueHandler());
  shared_ptr<TProcessor> processor(new ImageDownloadQueueProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}
