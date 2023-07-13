#!/usr/bin/env python3

#from datetime import datetime
import time
import datetime
from datetime import timezone
from elasticsearch import Elasticsearch
from ssl import create_default_context

context = create_default_context(cafile="./secrets/certs/ca/ca.crt")
es = Elasticsearch(
    'https://localhost:9200',
    ssl_context=context,
    # verify_certs=False,
    basic_auth=('elastic', 'changeme'),
)
print(es.ping())

doc = {
    'author': 'kimchy',
    'text': 'Elasticsearch: cool. bonsai cool.',
    # 'timestamp': datetime.now(),
    # 'timestamp': datetime.datetime.fromtimestamp(time.time(), datetime.timezone.utc).isoformat('T'),
    'timestamp': datetime.datetime.now(timezone.utc),
}
#resp = es.index(index="test-index", id=1, document=doc)
resp = es.index(index="test-index", document=doc)
print(resp['result'])

resp = es.get(index="test-index", id=1)
print(resp['_source'])

es.indices.refresh(index="test-index")

resp = es.search(index="test-index", query={"match_all": {}})
print("Got %d Hits:" % resp['hits']['total']['value'])
for hit in resp['hits']['hits']:
    print("%(timestamp)s %(author)s: %(text)s" % hit["_source"])

es.close()

# GET /_cat/indices
# CURL_CA_BUNDLE=/home/ubuntu/src/elastdocker/secrets/certs/ca/ca.crt curl https://localhost:9200/ -u elastic:changeme
# curl -k  -XGET https://localhost:9200/_cat/indices -u elastic:changeme
# curl -k  -XGET https://elastic:changeme@localhost:9200/_cat/indices
# curl --insecure  -XGET https://localhost:9200/_cat/indices -u elastic:changeme
# GET your_index/_search
# {
#   "size": 1,
#   "script_fields": {
#     "now": {
#       "script": "new Date().getTime()"
#     }
#   }
# }
