#ifndef __HTTP_CODES_H
#define __HTTP_CODES_H

typedef enum {
  Continue = 100,
  SwitchingProtocols,
  Processing,
  EarlyHints,

  OK = 200,
  Created,
  Accepted,
  NonAuthoritativeInfo,
  NoContent,
  ResetContent,
  PartialContent,
  MultiStatus,
  AlreadyReported,
  IMUsed = 226,

  MultipleChoices = 300,
  MovedPermanently,
  Found,
  SeeOther,
  NotModified,
  UseProxy,
  UNUSED ,
  TemporaryRedirect,
  PermanentRedirect,

  BadRequest = 400,
  Unauthorized,
  PaymentRequired,
  Forbidden,
  NotFound,
  MethodNotAllowed,
  NotAcceptable,
  ProxyAuthRequired,
  RequestTimeout,
  Conflict,
  Gone,
  LengthRequired,
  PreconditionFailed,
  RequestEntityTooLarge,
  RequestURITooLong,
  UnsupportedMediaType,
  RequestedRangeNotSatisfiable,
  ExpectationFailed,
  ImATeapot,
  MisdirectedRequest = 421,
  UnprocessableEntity,
  Locked,
  FailedDependency,
  TooEarly,
  UpgradeRequired,
  PreconditionRequired = 428,
  TooManyRequests,
  RequestHeaderFieldsTooLarge = 431,
  UnavailableForLegalReasons = 451,

  InternalServerError = 500,
  NotImplemented,
  BadGateway,
  ServiceUnavailable,
  GatewayTimeout,
  HTTPVersionNotSupported,
  VariantAlsoNegotiates,
  InsufficientStorage,
  LoopDetected,
  BandwidthLimitExceeded,
  NotExtended,
  NetworkAuthenticationRequired
} http_status_code_t;

/*
  Adquire a descrição de um código de status HTTP

  ATENÇÃO: a string retornada não deve ser modificada!

  @param status_code: código de status HTTP(ver http_status_code_t)
*/
const char* get_http_status_description(http_status_code_t status_code);

/*
  Verica se determinado status de resposta HTTP inclui ou não um body

  @param status_code: status a ser verificado
*/
int include_body(http_status_code_t status_code);

#endif // __HTTP_CODES_H