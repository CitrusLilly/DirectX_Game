//───────────────────────────────────
// テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────
Texture2D g_texture : register(t0); // テクスチャ
SamplerState g_sampler : register(s0); // サンプラー

//───────────────────────────────────
// コンスタントバッファ
// DirectX　側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────
cbuffer global
{
    float4x4 matWVP;        // ワールドビュー射影変換行列
    float4x4 matNormal;     // ワールド行列
    float4 diffuseColor;    // マテリアルの色
    int isTexture;          // テクスチャを張っているかどうか
};

//───────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────
struct VS_OUT
{
    float4 pos : SV_POSITION;   // 頂点シェーダー出力位置
    float2 uv : TEXCOORD;       // UV座標
    float4 color : COLOR;       // 色(明るさ)
};

//───────────────────────────────────
// 頂点シェーダー
//───────────────────────────────────
VS_OUT VS(float4 pos : POSITION,float4 uv : TEXCOORD,float4 normal : NORMAL)
{
    // ピクセルシェーダーへ渡す情報
    VS_OUT outData;
    
    // ローカル座標に、ワールド・ビュー・射影行列で変換
    outData.pos = mul(pos, matWVP);
    // UV座標をピクセルシェーダーへ
    outData.uv = uv;
    
    // 法線ベクトルをワールド行列で変換
    normal = mul(normal, matNormal);
    
    // 色(明るさ)をピクセルシェーダーへ
    float4 light = float4(0, 1, -1, 0); // ライトの向き
    light = normalize(light); // 正規化
    outData.color = clamp(dot(normal, light),0,1); // 法線ベクトルとライトの内積計算
    
    // まとめて出力
    return outData;
}

//───────────────────────────────────
// ピクセルシェーダー
//───────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
    float4 lightSource = float4(1.0, 1.0, 1.0, 1.0); // 光源の色
    float4 ambentSource = float4(0.2, 0.2, 0.2, 1.0); // 環境光の色
    float4 diffuse;
    float4 ambient;
    
    if (isTexture == 1)
    {
        diffuse = lightSource * g_texture.Sample(g_sampler, inData.uv) * inData.color; // 拡散光
        ambient = lightSource * g_texture.Sample(g_sampler, inData.uv) * ambentSource; // 環境光
    }else{
        diffuse = lightSource * diffuseColor * inData.color;
        ambient = lightSource * diffuseColor * ambentSource;
    }

    return diffuse + ambient;
}