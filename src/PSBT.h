#ifndef __PSBT_H__
#define __PSBT_H__

#include "Bitcoin.h"

// TODO: 
// - SIGHASH types, and other key-value pairs that are still not implemented
// - finalize()

/** \brief Derivation information */
typedef struct{
	/** \brief PublicKey that we are going to derive */
    PublicKey pubkey;
    /** \brief Fingerprint of the root HD key */
    uint8_t fingerprint[4];
    /** \brief Derivation path */
    uint32_t * derivation;
    uint8_t derivationLen;
} PSBTDerivation;

/** \brief Partial signature */
typedef struct{
    /** \brief Public key for the signature */
    PublicKey pubkey;
    /** \brief Signature we generated */
    Signature signature;
} PSBTPartialSignature;

/** \brief Data required for input signing */
typedef struct{
	/** \brief TransactionOutput data - scriptPubkey and amount */
    TxOut txOut;
    /** \brief Derivation paths for the keys required for signing.
     *         Can be more than one in case of multisig.
     */
    PSBTDerivation * derivations;
    uint8_t derivationsLen;
    /** \brief Redeem script for P2SH */
    Script redeemScript;
    /** \brief Witness script for P2WSH */
    Script witnessScript;
    /** \brief Signatures we will generate. In most cases will be just one. */
    PSBTPartialSignature * signatures;
    uint8_t signaturesLen;
} PSBTInputMetadata;

/** \brief Data required to confirm ownership of the output */
typedef struct{
    /** \brief Derivation paths for the keys.
     *         Can be more than one in case of multisig.
     */
    PSBTDerivation * derivations;
    uint8_t derivationsLen;
    /** \brief Redeem script for P2SH */
    Script redeemScript;
    /** \brief Witness script for P2WSH */
    Script witnessScript;
} PSBTOutputMetadata;

/** \brief Calculates descriptor checksum for Bitcoin Core. */
size_t descriptorChecksum(const char * span, size_t spanLen, char * output, size_t outputSize);

/** \brief PSBT class. See [bip174](https://github.com/bitcoin/bips/blob/master/bip-0174.mediawiki) */
class PSBT : public Streamable{
protected:
    virtual size_t from_stream(ParseStream *s);
    virtual size_t to_stream(SerializeStream *s, size_t offset = 0) const;
    Script key; // key for parsing
    Script value; // value for parsing
    uint8_t current_section;
    size_t last_key_pos;
public:
    virtual size_t length() const;
    PSBT(){ txInsMeta = NULL; txOutsMeta = NULL; status = PARSING_DONE; };
    PSBT(PSBT const &other);
    ~PSBT();
    Tx tx;
    PSBTInputMetadata * txInsMeta;
    PSBTOutputMetadata * txOutsMeta;

    /** \brief adds key-value pair to section */
    int add(uint8_t section, const Script * k, const Script * v);
    /** \brief Signes everything it can with keys derived from root HD private key */
    uint8_t sign(const HDPrivateKey root);
    /** \brief Calculates fee if input amounts are known */
    uint64_t fee() const;

    PSBT &operator=(PSBT const &other);
};

#endif // __PSBT_H__