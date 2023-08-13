# Still works in UE5.3
UE 5.3.0 Preview checked

https://youtu.be/BEjDlk0Z9go
https://youtu.be/jUSSy6UnNxc 
https://www.youtube.com/shorts/iVvXAq4qD00

# Installation
1. Install VS2022 
2. Create a  C++ Unreal Engine Project, close.
3. Get plugin socue code and glm:
 - run cmd in [Project Root]\Plugins
 - git clone --recurse-submodules https://github.com/axilesoft/IM-for-UE5.git
4. Right click UE project file (*.uproject),  select "Generate Visual Studio project files".
5. Open VS project compile , run.

A tool to convert leg-IK to leg-D animation (since UE animation has no IK)
https://github.com/axilesoft/IM-for-UE5/releases/tag/vmd_tool_v0.1
------------
# IMPORT MMD for UE5

Main improvement
 - Import PMX/VMD to UE5
 - Fixed material importing
 - Fixed morph importing
 - Fixed vmd importing slow problem
 - Generate rigid body based on pmx physics data

Not supported yet
 - Leg IK (Control Rig auto generating)   

# ORIGINAL README

Import [MikuMikuDance (MMD)](https://sites.google.com/view/vpvp/) models and motion files plugin for Unreal Engine 4.

PluginDev branch mirror of https://github.com/goopymoon/UnrealEngine_IM4UPlugin/
(Requires GitHub account with [access to Unreal Engine 4 source code](https://www.unrealengine.com/en-US/ue4-on-github) to see).

Original plugin by [BlackMa9(bm9)](https://github.com/bm9/).

Original repository contributors:
* goopymoon https://github.com/goopymoon/
* alwei https://github.com/alwei
* demuyan https://github.com/demuyan

This software is released under the MIT License.

This repository is public and doesn't requires a GitHub account with access to Unreal Engine 4 source code to see it. You can add this repository as a git submodule directly into your Unreal Engine 4 project plugins folder.Cancel changes

Latest Unreal Engine 4 version tested: **4.25.1**

# Get Start

1. GitHubからIM4Uのプラグインソースを丸ごと取得
2. 取得したソースから導入したいUEのプロジェクト(C++)の「Plugins」フォルダに本プラグイン「IM4U」フォルダ毎コピーする
3. プロジェクトを起動し、プラグインのビルドを実行させる
4. ビルドが完了しエディタが起動した際に、プラグインでIM4Uがインポートされていることを確認
5. ContentsBrowser内にMMD系ファイル(pmx,pmd,vmd)をインポートし各表示にしたがってAssetを生成する

UE4にMMDで作成したモデルやVMDファイルをそのままDrag&Dropでインポートすることが出来ます。  

# 制限事項および注意事項

本プラグインに関しては個人の趣味で作成したものであり、バグが含まれている可能性が十分あります。  
本プラグインを使用したことにより、如何なる損害が発生しても著作権保有者は保証義務を一切負わないものとします。  
PJ内のアセットを破壊やエディタの強制終了が発生してしまう場合がある為、事前及び定期的にバックアップをとっておくことを推奨します。  
今後、プラグインのVersionUPにより専用アセットのデータ構造を変更する場合があります。  
互換性を保証することは出来ない為、本プラグイン更新の前にPJのバックアップをとっておくことを推奨します。  

特に、第三者が作成したリソースを本ツールにてインポートする場合、  
必ず該当リソースに付属されるReadMeやコメント、規約事項について、  
リソース利用者が一読し内容に同意した上で使用すること。  
例えば、該当リソースについて本来想定するプラットフォーム(ツール)以外で使用することを明示的に禁じている場合は、  
その規約事項に従い本ツールでも使用しない事。  
(ただし、リソース制作者から事前に許可・同意を得ている場合、提示された条件・内容を優先し従う事)  
**上記の条件が守れる場合に本ツールを使用する事。**  

また、MMDとの完全互換性は難しい為、本プラグインを使用しリソースを取り込んでも再現出来ない場合があります。  
どうしてもそのリソースが必要である場合、他の手段で回避するor諦めて下さい。  
MMDに関しても理解不足の箇所もあり誤った実装をしている可能性もあります。  
本プラグインで作成するアセットはReImport処理未対応の為、一度ファイルを削除した上で再度Drag&Dropでインポートする必要があります。  
また、一部の文字がファイル名、ボーン名、Morph名などに入っている場合、UE4の都合によりツール内で別の文字に置換している場合があります。  

本ソースは**「UE4.16.x」**をサポートしています。
  
現時点で対応しているインポート処理は下記の各制限事項に記載通りとなります。  


## 制限：MMDモデルのインポート

- PMX,PMDファイルをDrag&DropすることでSkeletonMeshのAssetを作成します。  
　StaticMeshでのインポート処理は未対応です。  
　(ただし、拡張子をpmd_st,pmx_stとすることで、試作機能としてインポートすることも可能ですがライティングでビルドNGになります)  
- ~~PMD形式でインポートした際には内部でPMX形式に変換し処理していますが、  
　一部処理未対応によりデータ落ちしている場合があります。~~(Ver.0.6.7にて修正：Weight情報及びIK情報)  
- PMXは2.1形式でも読み込み可能であるが、制限事項に記載の通り一部取り込んでいないデータがあります。  
- アセット名はPMD,PMX内のモデル名称(JP)から読み取り生成しています。(ファイル名は現在参照していません。必要であれば今後対応予定)  
- PhAT(物理アセット)による剛体およびJointの再現は未対応(FBX読み込み時と同じ最低限の処理のみ実施)。  
- ボーンに関しては先頭に「~~ルート~~Root」という名称で親ボーンを追加し、UE4内にインポートしています。  (Ver.0.7.2にてBone名を変更) 
- ボーンの座標軸が手抜きのため、すべて同じ座標系となります。(ローカル軸未対応)  
- ~~ボーンのウェイトはBDEF1のみ対応しています。~~(Ver.0.6.7にて修正)  
　ただし、PMXのSDEF及びQSEFに関しては今後も未対応の予定です。暫定的にBDEF2及びBDEF4に置き換えて設定していますが動作未確認です。
- ボーンのIK設定は未対応(通常ボーンとしてインポートしています)  
　MMDExtendAssetにてIK情報のみを保存しているが、IK計算は未実装のため事前にベイクしたモーションを使用する必要あり。  
- Morphを読み込みたい場合は、ImportOptionでMorph欄にチェックを入れる必要があります(FbxImport時と同じ)  
- MMDの表情データに関しては頂点モーフのみ対応しています。  
- インポート時のマテリアル生成に関しては別途マテリアルの制限事項を参照。  
- インポート後法線マップの修正処理が無いため、モデルによっては法線マップがおかしい場合があります。  
　回避方法として、PMD/PMXエディタにて「不正法線の修正」を実施した上でインポートすることで法線マップをある程度再現できます。  
- モデルインポート時に、Ik情報等を外部アセット(MMDExtend)にて自動生成します。  
- MMDExtendAssetのモデルインポート時に設定する命名規則は、"モデル名"+"_MMDExtend"となる。  

他。  

## 制限：マテリアル

- PMD,PMXのモデルインポート時にデータ内に設定されている材質及びテクスチャ情報を元に自動生成しています。  
- マテリアルに関しては、知識不足によりお試しで実装しています。(間違ったマテリアル設定をしいます)  
- 彩度の低い色では透過されてしまう場合があり必要に応じて修正が必要となります。(Opacityにリンク接続しているため)  
- AutoLuminousは未対応となります。(ソース上ではEmissiveとして発光処理をテストで実装していますが、コメントアウト中)  
- スフィアマップは未対応。(UE側での再現方法が不明。情報があれば今後対応予定。)  
- エッジは未対応。必要に応じてLevel内にPostProcessにて追加で実装すること。  
- テクスチャのアセット命名規則は、"T_"+ファイル名(拡張子を除く) となります。  
　拡張子以外のファイル名が同じ場合正しくアセットを生成出来ない場合があります。  
　ファイル名に期待しない文字が挿入されている場合、正しくアセットを生成出来ない場合があります。  
　現時点では、必要に応じてファイル名とMMDモデル内のファイル参照情報を修正しておく必要があります。  
- マテリアルのアセット命名規則は、"M_"+材質名 となります。  
　ただし、PMXで取り込んだ場合、試験的にIndex番号を付与する方式に変更。("M_"+番号+"_"+材質名)  
　これはUE4内部で使用できない文字を使った場合に"_"へと置換しており、偶然マテリアル名が同一となった場合に、  
　正しく別マテリアルとして生成できるようにする為の対処です。(追々、ボーンなどでもチェックボックス判定により設定可能へと検討する予定)。  

他。

## 制限：VMDモーションのインポート

- VMDファイルをDrag&DropすることでAnimSequenceアセットを生成することができます。  
　この時ImportOption(Slate)にて対象のSkeletonMeshのアセットを選択する必要があります。  
　対象のSkeletonに該当ボーン及びモーフ名が存在しない場合はそのTrack(名称)のみ破棄されます。  
　回避策として、DataTable(MMD2UE4NameTableRow)を追加指定することで名称の読み替えを実施できます。  
　PMD・PMX側のボーン名とVMD側で保存するボーン名の文字数制限によりボーン名が15byte以上である場合、  
　検索時に完全一致で実装しているため該当ボーンのモーションを取り込めない場合があります。(バグのため、今後前方一致で実装する予定)  
- ただし、VMDインポート処理の仮実装をしていますが、  
　Ik処理が未実装及び未実装によりのIKに依存するモーションでは期待しないポーズとなるため、正式にはまだVMDは未対応とります。  
　インポート先のSkeletonの各ボーンの座標系によっては意図しないモーションになる場合があります。  
　同様に物理演算の挙動を再現出来ないため、例えばMMMやMMDBridgeなどのツールで事前にVMD内にIkや物理挙動を焼き込んでおくことで擬似再現が可能。  
- VMD内のKey及び補間曲線のデータからAnimSequenceのアセットを生成する際に、30FPSで各ボーンの位置を計算しKeyを打ち直しています。  
- VMD内の表情(モーフ)に関してはほぼ実装が完了しています。  
　既存のAnimSequenceに対し後付でMorphCurveを追加することができます。  
　モーフのみ後付でインポートしたい場合は、対象SkeletonMesh(必須)、追加AnimSequence(必須)、  
　DataTable(後述の名称対応表)(任意) を指定する必要があります。  
　この時、追加AnimSequenceに指定したアセットに直接MorphCurveを追加します。(事前に元アセット複製しておくことを推奨)  
- 後付モーフの場合、追加先のAnimSequenceアセットのフレーム数以上のVMDデータを追加インポートする場合、  
　超えたフレームのデータは破棄されます。  
- VMDインポート時に対象のSkeletonMesh内とVMD(MMD)側のボーン名及びモーフ名が異なる場合、  
　事前にcsv形式で対応表となるDateTable(MMD2UE4NameTableRow)アセットを作成しておく必要があります。  
　上記DataTableをVMDImportOputionに追加指定しておくことで、MMD->UE4向けに名称変換し該当する名称が存在する場合、  
　読み替えた上で該当Track(名称)のアニメーションをインポートできます。  
- 上記、FMMD2UE4NameTableRowのデータテーブル形式は「Name="UE4側の名称",MmdOrignalName="MMD側の名称"」でUTF-8となります。  
　ここでいう名称とは、ボーン名とモーフターゲット名を示しています。  
　例(csv)：bone1,新規ボーン1  
- 上記変換テーブルを作成しておくことで、例えばUnity側のツールMMD4Mecanimなどで作成した  
　Fbxアニメーションから作成したAnimSequenceアセットに対し、  
　UE4用のMorphCurveとしてモーフのみ後付で追加することが可能です。  
- VMDデータからAnimSequenceアセットを新規作成する場合の命名規則は、  
　”VMDのファイル名(拡張子を除く)”+"_"+"SkeletonMeshのアセット名" となります。  

他。

## FMMD2UE4NameTableRow(MMD-UE4間の名称変換表)について (Ver.0.6.5～) Ver.1.0

csvにUTF-8形式(SJISの場合、日本語の文字化けが発生します)で記載し、UE4のEditor上にDrag&Dropすることで  
DataTable(FMMD2UE4NameTableRow指定)を作成できます。  
このテーブルを使うことでVMDからAnimSequenceアセット生成時に内部で該当名称を読み替えてデータをインポートすることができます。  
一行目に「Name,MmdOrignalName」のデータ構造体？を必ず記載し、2行目から各名称のUE4-MMD間の対応関係を記述する必要があります。  
本csvにて記載する内容は、ボーン名とモーフ名を記載することを想定していますが、  
現在、VMDからモーションをインポートする処理が完全で無い為、  
既存のアニメーションアセットに対しモーフのみを後付する機能向けにモーフの対応関係のみ記載することを想定しています。  

また、記載する条件としてName<->MmdOriginが一対一で対応し重複しないこと。  

## ファイルの記述例：hogehoge.csv

> Name,MmdOriginalName  
122_!Root,ルート  
1_joint_Torso,上半身  
79_!joint_RightToe,右つま先  
70_joint_RightFoot,右足首  
69_joint_RightKnee,右ひざ  
68_joint_RightHip,右足  
0_?????????,真面目  
1_??????,困る  
2_?????????,にこり  

## MMDExtend Assetについて(Ver.0.6.6 ~) Ver.0.2

MMDExtend Assetはその名前通りMMD用のUE向け拡張データを保存する為の独自アセットです。  
通常のSkeletonMeshのアセットにMMD用用のIK情報などを持たせない設計方針としています。  
(拡張したアセットを自作することも可能であるがSkeletonを特化するとUE4のVer毎にメンテナンスが必要となる)  
この専用アセットが保有するデータは、モデルのヘッダー部分(モデル名、コメント文)IK情報となる。  
IK情報は、VMDデータインポート時にIK情報を設定した本アセットを追加していすることで、  
モーション作成時にIKの計算に使用します。  
したがって、VMDインポート時に本アセットを指定しない場合はIKの再現は実施しない事になります。  
なお、本アセットはPMD,PMXモデルインポート時に自動生成するが、右クリックから「未分類」ー>「MMDExtend」で新規作成することが可能です。  
  
データ型に関しては、「MMDExtendAsset.h」に実装していますので必要があれば確認して下さい。  
→Ikの実装ができていないため、例えば角度の条件値の持ち方などで修正が入る可能性大です。  
　このため、現時点では使用しない方向でお願いします。  
　IK計算はバグにより除外している為、VMDをモーションインポートでも本Assetを取り込めない様に変更しています。  

## Material instance対応について(Ver.0.7.3 ~)

ModelImport初期時の大量シェーダコンパイルの回避するため、  
初期のMaterial生成ロジックを変更しました。  
ImportOptionの「bCreateMaterialInstMode」を有効にすることで動作します。  
ベースとするMaterialはIM4UのContents内にあるAssetから複製します。  
このため、もしBase-Materialを変更したい場合は、プラグイン内のAssetを変更すればOKです。  
効果は、Material数40個ある場合に5000～9000くらいが、400~1000くらいまで落とせます。  
他、複製したMaterialを修正すれば、継承するMaterial-Instanceがまとめて編集できるので修正コストも減らせるはず。  
制限として、ModelImport失敗した場合古い一時ファイルが残ることが稀にあります。その場合は、リダイレクトで削除してください。  

また、「bUnlitMaterials」を有効にすると生成するMaterial-InstanceがUnlitベースになります。  
ライティングなしとなるため、UE4上では浮いた絵となりますがMMDオリジナルベースに近づきます。  

なお、ベースとするMaterialのAssetはMaterial初心者の作品なので、いいAssetのPullRequest募集中です。  


# ToDo

※進捗率は大まかな目安です

-  MMDモデル及びVMDファイルの互換性及び読み込み時のデータ落ち修正  
	⇒進捗率(55%)→完全な互換性に関しては難しいと推測。  
-  特殊ボーンのWeightの対応  
	⇒進捗率(80%)→PMXのSDEF及びQDEFはBDEF2とBDEF4に置き換えて試験実装中。完全互換は難しいと推測。  
-  MMDモデルの剛体からPhATアセットの作成処理  
	⇒進捗率(1%::標準アセットのみ生成)  
-  VMDインポート処理(IKおよび物理挙動、ボーンのアニメーション)  
	⇒進捗率(65%)->ただしIKに関しては殆ど出来そうにない…→MMDBridgeを使うことでベイク済みIKとして擬似再現可能。  
-  各Assetインポート時のGui(Slate)の作成(大半の項目はダミーの為)  
	⇒進捗率(90%)→ただしいヘルプ（コメント）が対応できていない。  
-  マテリアルの対応(透過処理など)  
	⇒進捗率(80%::サンプル不足、一部透過処理にバグ、スフィアマップの設定方法募集中)  
-  各AssetのRe-Import対応  
	⇒進捗率(0%)  
-  ~~VMDインポート時にターゲット(Skeleton)のボーン/Morph名とVMD内のボーン/Skin名との対応付機能。(外部FBX変換ツールとの互換用)
	⇒進捗率(100%)~~  
-  専用アセットの検討(MMDExtend,FMMD2UE4NameTableRowなど？)  
	⇒進捗率(20%)  
-  ソースのリファクタリング  
	⇒進捗率(0%)：進捗ダメです  
-  ソースのフォルダ階層整理  
	⇒進捗率(0%)：進捗ダメです  
-  コンパイル時間の短縮  
	⇒進捗率(0%)：リファクタリング項目と同様

などなど


# 確認済みのバグ

1. ~~MMDモデルを直にインポートして作成したモデルにアニメーションを追加した場合、期待しない頂点の位置となる。  
　→現在対応しているボーンのWeight種別が1種類のみである為、見栄えが悪くなります。~~←左記に関してはVer.0.6.7にて修正。  
　　プラグインによるVMDインポート機能はまだ未完成です。  
2. ~~VMDのモーションを直にインポートした場合、例えばZ軸（上）でターンするモーションで  
　上半身と下半身の回転方向が物理的にありえない組み合わせでねじれて見える。  
　→クォータニオンの使い方が間違っていると思われますがまだ修正出来ていません。~~←左記については、Ver.0.6.12にて修正。   
3. モデルインポート時に生成するマテリアルアセットを使った場合、一部の暗色系が透過扱いになってしまう。  
　→自動生成しているマテリアルノードの作りに問題があるため期待しない透過となっています。  
　　MMD向けのマテリアルノードの組み方の一般解がわかれば今後対応可能です。  
　　→回避方法としては、期待しないマテリアルがあれば手作業で修正して下さい。  
　　→スフィアマップをUE4内で再現する為のマテリアルノードの設定方法があれば情報下さい。  
4. VMDインポート時にMMDExtendAssetを指定するとIK計算処理でクラッシュが発生する場合がある。  
　→現時点では内部でのIK計算はサポートしていないため、VMDインポート時に指定する追加アセットから除外。(Ver.0.6.9)  
5. VMDインポート時に一部のモーションで期待通りに座標計算できていない場合がある。(Ver.0.6.12～)  
　→特に、IKベイク済みVMDを取り込んだ場合、足先の座標が左右にブレて滑っているような状態となる。  
6. ~~UE4.9.x～環境の場合、SkeltonAssetとの関連付けが正しく設定できない。(Ver.0.6.13～)  
　→インポート後の初期回アクセス時に、スケルトンが存在しないと通知される。Editorの指示に従い新規ファイルを作成すれば使用可能。~~←左記については、Ver.0.6.14にて修正。  
7. 特定のボーン構造のモデルをインポートすると必ずクラッシュする。(Ver.0.6.13-2～)  
　→原因：ボーンの定義で親ボーンIndexが参照している子ボーンより後ろにデータ定義されている場合、そのままUE4で取り込むと現状ソートし直さない為スケルトンのチェックで異常と見えてしまう。  
　→暫定回避策：MMDモデルのボーン構造を修正する。  
　→対象モデル：おんだ式ハッカドール1号 など  
　→暫定対処：NGフォーマット構造の場合、警告文を表示させクラッシュする恐れがある旨のMessageBoxにて表示するよう対応。(Ver.0.6.14～)
  →暫定対処：MessageBoxを無視して読み込んでもCrashしないようにガード処理を追加。(Ver.0.7.2～)    


# Release Note

## ver 0.6.0 15/04/01

- 1st alpha ver. release  
- UE 4.7.x系でのみ対応開始  

## ver 0.6.1 15/04/05

- バグ修正  

## ver 0.6.5 15/04/26

- VMDインポート機能の試験実装(IKおよび物理挙動は未対応)  
- VMDインポート機能追加により既存AnimSequenceアセットに後付でモーフのみ追加することが可能  
- 各データインポート時に簡単な制限事項のSlate表示を追加  
- SkeletonMesh作成時の親Boneの名称を「ルート」名に変更  
- FMMD2UE4NameTableRowのデータテーブル型を追加
- 他、バグ修正  


## ver 0.6.6 15/05/09

- PMX,PMD,VMDImport時に表示されるUI(Slate)にて使わないデータに関しては非表示するように変更。  
- PMX,PMDからのモデルImport時に、上記UIにて「ImportMaterials」、「ImportTextures」の項目を参照するように追加。  
　(OffでTextureもしくはMaterialのアセット生成をしないことが可能。FBXインポートと類似動作)  
- 今後の機能盛り込みに合わせ、先行でIK計算用に新規独自アセットを追加(MMDExtendAsset Ver.0.2)。  
　PMD,PMXモデルインポート時にバイナリデータから自動生成。新規アセット生成から単独で空アセットも作成可能。  
- モデルインポート時に参照していたIK情報のバグを修正(IK利用時は再読み込みを推奨)  
- VMDインポート時、事前にIK計算する処理を試験実装(ただし未完成でバグが含まれる為非推奨）  
　→VMDインポート時に表示されるUIにて「MMDExtendAsset」のアセットを選択せずNULL状態にしておけば、  
　　IK計算処理に入らずに通常処理だけ実行できます。（こちらもバグがあるので非推奨ですが- - - ）  
- FMMD2UE4NameTableRowのデータ構造体うち、「MmdOriginalName」名のスペルミス(iが抜けてた)を修正。  
　必要に合わせ再度CSVをインポートする場合は修正が必要。  
　→誤「MmdOrignalName」→正「MmdOriginalName」  
- UE4.8Pre以降で利用する場合は、一部ソースの修正が必要。  
　→「IM4U\Source\IM4U\Private\PmxSkeltalMeshImport.cpp」の  
　　FAsyncImportMorphTargetWork::GetStatId関数の#if 0を解除すること。  
- 他、ReadMeの制限事項やソースのリファクタリング等一部修正あり。  

## ver 0.6.7 15/05/17

- PMDおよびPMXモデル読み込み時に頂点に対するWeight設定でBDEF1にしか対応していなかった箇所で、  
　BDEF2,BDEF4も対応するように修正。ただし制限事項として、SDEF及びQDEFは再現が難しいと判断しBDEF2とBDEF4に内部置換で暫定対応。  
- マテリアル生成の命名規則を一部変更。Indexをアセット名に付与。  
- 4.8以降で本ツールを使用する際にソース修正が必要な箇所についてメモを追記。ただし、正式版でない為サポート外。

## ver 0.6.8 15/05/17

- PMD読み込み時のモーフ設定に誤りがあり、モーフターゲットがほとんど反応しなかったバグを修正。  
　→以前のVerで表示されていたBaseモーフを表示しないように同時修正。  
- PMD読み込み時にテクスチャファイルの判定で通常のテクスチャとスフィアマップが分離されず、正しく処理出来ていなかったバグを修正。  
　→これにより、スフィアマップのみ設定されたマテリアルの場合、マテリアルノードでスフィアマップをベーステクスチャとして参照していたバグが修正されます。  
　　スフィアマップを参照しなくなった為、以前のVerで取り込んだ際のマテリアル設定と異なるマテリアル設定がされる場合があります。  
　　いずれにせよ、現時点ではマテリアルノードの再現方法が確立出来ていないため、手動で調整する必要があります。  
　→制限としてこれまでと同様に、スフィアマップの対応に関してはマテリアルノードの設定方法が判明するまでマテリアルノードでは参照せず未対応予定。  
- 仕様変更：PMD読み込み時にマテリアル設定にて、アルファ値に関係なく必ず両面描画する設定に変更。  
　→PMDでアルファ値＝1.0の場合、本来裏地を表示しないはずだがUEの場合完全に透過して見えており、  
　　MMDと比較して角度によっては大きく見え方が変わってしまうので暫定対処としてデフォルトで両面表示に変更。  
- PMDインポート時にモデル内に設定されている日本語のコメント文を参照しておらず、Slateに表示されていなかったバグを修正。  


## ver 0.6.9 15/05/18

- モデル取り込み時のUIにて「bImportMorphTargetsを有効」にした場合、自動生成される全てのマテリアルアセットに対し、  
　デフォルトで「bUsedWithMorphTargets」のフラグを有効にするように変更。  
- VMDのファイル読み込み時にMMDExtendAssetを指定した状態でインポートすると、IK計算でクラッシュする場合があるためSlateに表示しないように変更。  
　→現時点で内部でのIK計算にバグが有り、誤ってアセット指定したことによるクラッシュ発生を避けるための暫定対処。  


## ver 0.6.10 15/05/31

- UE4エンジンビルドで本プラグインをビルドした場合にビルドエラーになる問題を修正  
　→Pull Request #1 (Ref: https://github.com/bm9/UnrealEngine_IM4UPlugin/pull/1)  
- MMDExtend Assetのクラス名をMMDExtendからMMDExtendAssetに変更  
　クラス名変更に合わせ、MMDExtendのヘッダー/ソースファイル名をMMDExtendAssetに変更。  
　→issue #1 (Ref: https://github.com/bm9/IM4U/issues/1 )  
- MMDExtendAssetでIKBoneIndexとBoneIndexをアセットの詳細パネルに表示し、  
　MMDのモデルインポート時に該当BoneNameからIndexを参照するように追加  
　→issue #2 (Ref: https://github.com/bm9/IM4U/issues/2 )  
- UE4本体のエンジンソースはリポジトリから除外  
　→以降、エンジンでのプラグインビルド確認は実施しない  


## ver 0.6.11 15/06/14

- UE4.8.x向けにブランチを作成  
- Add: [issue#2-2] MMDExtendAssetでTargetBoneIndexをアセットの詳細パネルに表示及びIndex値を参照する様に追加


## ver 0.6.12 15/06/28

- Fix: モーションインポートで事前計算(回転の補間計算)バグを一部修正  
　→上記修正により、既知問題(1)であるZ軸(UP)における想定外のひねりが含まれるバグが解消される。  
　→制限事項(5)：ただし、まだ座標計算にバグがあり一部のモーションでは期待通りにならないバグが含まれている。  
- Add: モデルインポート時に物理アセットを生成するしないのフラグを設定できるように追加  
　→本家、FbxInport時に表示されるOptionと同じ機能となる。  
- Fix: UE4.8環境でコンパイル時に出るWarmingを修正  
- Fix: フォルダ階層を変更(Plugins/Developer/IM4U → Plugins/IM4U)  


## Ver 0.6.13 15/10/18

- UE4.9.x向けにブランチを作成
- Bug: PMD,PMXモデルからSkeletonAssetを生成した際に、Skeletonが正しく関連付けできないバグがある  
　→暫定対処: skeltonの参照がおかしい為、MMDExtendAsstesの自動作成を一時コメントアウト  
　→エディタで初回起動する際にファイルが存在しないと通知されるが、新しく作成すれば新しいAssetで関連付けができる。  


## Ver 0.6.14 16/01/10

- UE4.10.x向けにブランチを作成
- Fix: [既知問題-6]UE4.9以降での本プラグイン使用で、モデルインポート時にスケルトンアセットが正常に生成できなかったバグを修正。  
- Add: [既知問題-7][暫定対処]モデルインポート時に想定外のフォーマットの場合、読み込みを継続するかをMessageBoxにて表示させる機能を追加。  


## Ver 0.6.15 2016/04/02

- Fix：[IM4U-issue＃6]windows依存を削除（"BYTE"→"uint8"、"WORD"→"uint16"に変更した）  


## Ver 0.6.16 2016/04/07

- UE4.11.x向けにブランチを作成
- Fix：UM4.11ビルド向けにコンパイルエラー箇所を修正。


## Ver 0.7.0 2016/05/18

- Fix：Texture付きMaterialで拡散色の使い方が間違っていたノード構成を修正。  
- Fix：MaterialでVectorParameterの名称をNoneから「DiffuseColor」と「AmbientColor」に変更。(※Materialinstance向け対応)  


## Ver 0.7.1 2016/05/18

- UE4.12.x向けにブランチを作成  
- Fix：UM4.12ビルド向けにコンパイルエラー箇所を修正。  


## Ver 0.7.2 2016/10/09

- UE4.13.x向けにブランチを作成  
- Fix：UM4.13ビルド向けにコンパイルエラー箇所を修正。 
- Fix:VMDImport時に表示されるMessageBoxのTitle用変数の重複を修正。  
- Chg:SkeletonMesh作成時の親Boneの名称を「Root」名に変更 (0.6.5の再修正)For Rig。  
- Up :UE4オリジナルコード依存箇所のSkeltalMeshImport.cppベース版数を4.9から4.13系に差し替え


## Ver 0.7.2 2016/11/13

- Fix:[既知問題-7][暫定対処]Bone階層が反転していた場合にクラッシュする前にチェックする処理を追加    


## Ver 0.7.3 2016/11/24

- Add:[Experiment]Material生成処理にプラグイン内部に格納したベースMaterialから複製し、Material-Instanceを生成する機能を追加。    
- Add:[Experiment]IM4UのプラグインのContentsフォルダを作成し、ベースMaterialを配置。  
- Add:[Experiment]Material-Instance生成モード有効、かつ、UnlitModeを有効にするとshaderModelをUnlitにする機能を追加。  


## Ver 0.7.4 2017/1/1

- Up :[Experiment]Material-Instanceを生成する機能の暫定リリース版に更新。  
- Fix:[Experiment]プラグイン用Material AssetのEmissive関係のノード構成を修正。 
- Fix:[Experiment]プラグイン用Material Assetのコメント分にBasePathを追加。    
- Fix:[Experiment]プラグイン用Material AssetのLuminous系のインポート時に設定する値の計算式を修正。    


## Ver 0.7.5 2017/1/9

- UE4.14.x create branch 
- Fix:UM4.14ビルド向けにコンパイルエラー箇所を修正。 
- Fix:VMD Import時に最大Key数が想定より無駄に含まれるBugを修正。    


## Ver 0.7.6 2017/1/10

- Add:VMD Import時にVer.0.7.5で変更したSkeletalMesh未参照バグを修正するために、新規でSkeletalMeshを追加した。 
- Add:VMD Import時に本Verから追加したSkeletalMeshの選択が異常な場合にリトライ処理をするように追加。    
- Add:VMD Import時にSkeletonとSkeletalMeshが不一致の場合、ImportERRにするガード処理を追加。    
- Fix:VMD Import時のImportOptionUIを修正し新規作成した。      
- Fix:Ver.0.7.5で発生したAnimationAssetのPreviewMeshが正しく表示されるように修正。    
- Fix:PMX/PMD Import時のImportOptionUIから不要な項目を削除した。     
- Fix:ImportOptionUIに設定されていた不要なFBX関係のOnlineHelpeを削除した。    


## Ver 0.7.7 2017/5/7

- :arrow_up:UE4.15.x create branch 
- Fix:UM4.15ビルド向けにコンパイルエラー箇所を修正。 
- :art::checkered_flag:Add:4.15系のIWYU対応によるwindows.h Include Err回避のために、Lib化(LibEncodeHelperWin)を実施。  
   Dir::"ThirdParty\LibEncodeHelperWin"  


## Ver 0.7.8 2017/5/7

- :arrow_up:UE4.16.x create branch 
- Fix:UM4.16(Pre2)ビルド向けにコンパイルエラー箇所を修正。 


以上
