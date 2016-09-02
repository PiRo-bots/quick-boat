window.onload = function () {
  console.log('here')
  document.getElementById('connectionStatus').text = 'disconnected'
  const rudderAngleV = document.getElementById('rudderAngle')

  // open a web socket
  const host = '192.168.4.1' // + window.location.hostname
  const port = 81
  const socksrv = `ws://${host}:${port}`
  const ws = new WebSocket(socksrv)

  document.getElementById('rudder').onchange = function (e) {
    const value = e.target.value
    ws.send('r' + value)
    rudderAngleV.value = value
  }

  document.getElementById('rudder').oninput = function (e) {
    const value = e.target.value
    ws.send('r' + value)
    rudderAngleV.value = value
  }

  // When Web Socket is connected
  ws.onopen = function (evt) {
    console.log('ws (re)connect')
    document.getElementById('connectionStatus').innerHtml = 'connected'
  }

  // When websocket message
  ws.onmessage = function (evt) {
    console.log('message', evt)
  // var obj =  eval( "(" + evt.data + ")" )
  // var msg = obj.message
  // var data = obj.data
  }

  // When websocket is closed.
  ws.onclose = function (code, reason) {
    document.getElementById('connectionStatus').innerHtml = 'disconnected'
  }

  // When Web Socket error
  ws.onerror = function (evt) {
    document.getElementById('connectionStatus').innerHtml = 'error'
  }
}
