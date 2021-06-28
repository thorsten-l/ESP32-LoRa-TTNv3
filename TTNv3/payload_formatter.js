function decodeUplink(input) {
  var data = {};
  
  data.message = String.fromCharCode.apply(null, input.bytes);
  
  return {
    data: data,
    warnings: [],
    errors: []
  };
}
