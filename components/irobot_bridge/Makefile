
ca_cert_chain.pem:
	openssl s_client -showcerts -connect 10.0.2.92:8883 </dev/null 2>/dev/null | sed -n '/-----BEGIN CERTIFICATE-----/,/-----END CERTIFICATE-----/p' > ca_cert_chain.pem

ca_cert_chain.cpp: ca_cert_chain.pem
	 xxd -i ca_cert_chain.pem > ca_cert_chain.cpp



