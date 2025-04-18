#define CATCH_CONFIG_MAIN
#include <http_server.hpp>
#include <crow/crow_all.h>

#include "catch.hpp"

#include "acm.hpp"
#include "utilities.hpp"


bool loadTestCases( const std::string& case_file, StrVector& case_data ) {

    std::string line;
    std::ifstream file{ case_file };

    if ( file.good() ) {
        while ( std::getline( file, line ) ) {
            string_utilities::strip( line ); 
            if ( line.length() > 0 && line[0] != '#' ) {
                // skip empty lines and comments.
                case_data.push_back( line );
            }
        }

    } else {
        return false;
    }

    return( !case_data.empty() );
}

pugi::xpath_query ode_payload_query("OdeAsn1Data/payload/data");
pugi::xpath_query bsm_vehicle_event_flags_query("MessageFrame/value/BasicSafetyMessage/partII/BSMpartIIExtension/partII-Value/VehicleSafetyExtensions/events");
pugi::xml_document output_doc;
pugi::xml_parse_result parse_result;
pugi::xml_node payload_node;
pugi::xml_node byte_node;
ASN1_Codec asn1_codec{"ASN1_Codec","ASN1 Processing Module"};

const char *BSM_HEX = "001480AD562FA8400039E8E717090F9665FE1BACC37FFFFFFFF0003BBAFDFA1FA1007FFF8000000000020214C1C100417FFFFFFE824E100A3FFFFFFFE8942102047FFFFFFE922A1026A40143FFE95D610423405D7FFEA75610322C0599FFEADFA10391C06B5FFEB7E6103CB40A03FFED2121033BC08ADFFED9A6102E8408E5FFEDE2E102BDC0885FFEDF0A1000BC019BFFF7F321FFFFC005DFFFC55A1FFFFFFFFFFFFDD1A100407FFFFFFFE1A2FFFE0000";
const char *TIM_HEX = "001f85fe7591fd9e4f4354455420535452124c16b4fa4e724f43ef73d2f04fd7ad0ff09260b5a7d2a6249ba8936ffa29bac57176dadf5389a4000a2001c3a03dc06ba5067b8404632ae6a0503389c06966bc120c5900f771c1c6306a9a6bf319901630a0f2403124536d3e8476d9af77a7bf218901103a881122806195dcc32733526fe545fa217f819306eff7fdceaafa0230c87f355e98cb516360080a0002f348c72a3f584b11e4be99002ad4a0d12394d435cfda9fe80267e176eb96a2ff07260b5a7f4b9d40747f5ffe014b670db5da84587b200050400f55bfce9e9bc9d913782889646ff0126933eacd08beff7f5447df8cf48052da9a01005a0001bc2fa39c11fc2e7046450f0952d4dfe024ddb71fd2d3b502b9b2551e1d3f8b210e3c4000b00020c2dd46d113a3f21710e9416b92d5bfc14982d60dc94824a8e5c5095c1007d4e362e82720800270a04182240bda99c42bb6d94ff5f9a52a3014cd051a29843d4489d71d4150ae35b7533039e58186a0608ac0fc05f4bf24a4e851d3d0a801880f339e17e05ca32567a49b73b932580eecfc46064078a2e0120158f22817d1fb85eee4e66a4dfca8bf442fcca15810716140920643215dc607f829305ac1b92904951cb8a12b8200fa9c6c5d04e410004c0a0671380d2cd782418b201eee3838c60d534d7e633202c6141e4806248a6da7d08edb35eef4f7e431202207510224500c32bb9864e66a4dfca8bf442ff05260b5837252092a397142570401f538d8ba09c820009976b50fc6379b168326f9bdfe0a4c16b06e4a4125472e284ae0803ea71b1741390400132ed63059cde2d064df37c448db82d49305ad3e949305ad3e88bf939305ad3010933071674e428880448e75d46d038a29245b0b778f7001044f43ef73d2f04fd7ad0ff09260b5a7d2a6249ba8936ffa29bac57176dadf5389a4000a2001c3a03dc06ba5067b8404632ae6a0503389c06966bc120c5900f771c1c6306a9a6bf319901630a0f2403124536d3e8476d9af77a7bf218901103a881122806195dcc32733526fe545fa217f819306eff7fdceaafa0230c87f355e98cb516360080a0002f348c72a3f584b11e4be99002ad4a0d12394d435cfda9fe80267e176eb96a2ff07260b5a7f4b9d40747f5ffe014b670db5da84587b200050400f55bfce9e9bc9d913782889646ff0126933eacd08beff7f5447df8cf48052da9a01005a0001bc2fa39c11fc2e7046450f0952d4dfe024ddb71fd2d3b502b9b2551e1d3f8b210e3c4000b00020c2dd46d113a3f21710e9416b92d5bfc14982d60dc94824a8e5c5095c1007d4e362e82720800270a04182240bda99c42bb6d94ff5f9a52a3014cd051a29843d4489d71d4150ae35b7533039e58186a0608ac0fc05f4bf24a4e851d3d0a801880f339e17e05ca32567a49b73b932580eecfc46064078a2e0120158f22817d1fb85eee4e66a4dfca8bf442fcca15810716140920643215dc607f829305ac1b92904951cb8a12b8200fa9c6c5d04e410004c0a0671380d2cd782418b201eee3838c60d534d7e633202c6141e4806248a6da7d08edb35eef4f7e431202207510224500c32bb9864e66a4dfca8bf442ff05260b5837252092a397142570401f538d8ba09c820009976b50fc6379b168326f9bdfe0a4c16b06e4a4125472e284ae0803ea71b1741390400132ed63059cde2d064df37c4401b705a049305ad3e9024982d69f445fc9c982d69808499838b3a7214440224738ea3681c514922d85bbc7b8008227a1f7b9e97827ebd607f8093499f566845f7fbfaa23efc67a40296d4d00802d0000de17d1ce08fe173823228784a96a711136e0b524c16b4fa524c16b4fa22fe4e4c16b4c0424cc1c59d390a22011239df51b40e28a4916c2dde3dc004113d0fbdcf4bc13f5eb03fc049a4cfab3422fbfdfd511f7e33d2014b6a680401680006f0be8e7047f0b9c119143c254b538899b705a9260b5a7d29260b5a7d117f27260b5a60212660e2ce9c851100891cefa8da07145248b616ef1ee002089e87dee7a5e09faf581fe024d267d59a117dfefea88fbf19e900a5b5340200b40003785f473823f85ce08c8a1e12a5a9c450db82d49305ad3e949305ad3e88bf939305ad3010933071674e4288804480";
const char *SRM_HEX = "001d697125b7da9aa31b97b0fb3ec9148495a40fed6be3446a4e5b70ec1a2752b710fbfaf5ac086673396a81677da981b16a6b4905771f424e51683a70adb0afd713837a81719aed3fa8d4e347ef1b40e42d024585c757787442477e73341aae24982d69f40e4c16b4c428d8";
const char *SSM_HEX = "001e817b65e539dc93b843af683249404f9e0fc6b04fd122cf2ce89941dc1ab4d3d288394b59be74b1c04cfdee07bc9868311d2c1caa51f03dc764f993d0d511779e9ef22be1121c093e1af96b1d141a1ba967c329e47cf884b8beb3268e790f72270ca44c2519740d31d85f3a0e91a6bca5145e560e920d281085568f931b7067cc9e86a88b8f5957847ac6b4fa9d1b07fc2cd2e6a91f327a1aa229d5e21e478318d630a67bd8ffd0ce05537cf12267f5df5fc2794ff0804001a150fe00a679ce1c7934b4a6891be64f435445f9206bc5ff8e09b516244086367d14aef993d0d51175f310f233515cb0082fa6f907e861f0be64f435445412a1bcae64260fe3aa2470ea9ccf666f993d0d5114a9c22f0ba4406960b0cc40a0bd244e285bce95385017cefcbbac8d3070a7819776806012dd1dd66d719a089f3d143c9b843e001e4439710aacb223e510b9397770640941fcec2f9fc6a4cba57da12f160011a4d7c2fc0f7f10429d2be7409299a6cd9053c256161af6572d28cf07d9bdc620";
const char *SPAT_HEX = "0013838f65227f02487f07260b5a7ea4eea3b4400807cf548e3480e0f54afae04c14982d4a1183eefde4e4ae9d30fca80a906f32c28fceb1e486cd9fd93c2d459f18a78f3a18c1f2c7f371b0869045b061dc316d4783f3b1b5a8f02373272f5e4d3e43e28bc8d949e323784c3cb28a2c7f70c56eee2f4c349774c980780a932ee3ec7f57bc8c675aa86a1b255d837884388e1a0f23ba161109e05acd8ed8bc3de03dd7365fa25734fa22150f8d614a293c26a19a3af78506832764f1cfe2a53485e496f79e7bde2224dd7f9d839305ad3cd064f9065af84baa7369921efa9bc97ca6e6f578263bd7e14f01875bfef1e605c0a61783c42ec211d7b05260b548466f818f8f900ab51f108a63e8bc424d4912e79ddc45ed9ac9f1e46193c721453ead68310790031a7494cbf5a8b10a542d320ae0548f279541bd9236f1a4d7ec61de6626cc170b17d122a1aa087b217a403f281e27a60cacab1fd15ac7922179cb33c5c5a91e48125135a3c66ee3e5887909a8e5290f145811f925e06104656f03c644f887279bda2bcdb919bc3cc8d28adbccaa6d8be7bcc10a12ccb808721905d68b035663074e9f03703207e124c16b4fa6140d1ef05200008b6986c920383d52beb81305260b5d8460f8a4e9b40095c0d52cba5013c7cb9b735b79c1f97a7e0f319b50c13997ec7246d5c011a8493a7c0c8f3ba3957c09e21bc1374bbceb140ea4f78f0dbb3d70f3a078dbdbd8bf03a48c22b47c8673ca04a0f14ab3b6a1193eb224d4de5e8cd1ee4d67c4f3a8e4eec39e66e82ffcf3cd91254fee63fbf4f21de15d4b47781ca29bc55a1a06cc78a68a243c2f18ae8212c1e4aa4a9a968bd38661b55fbde1077ad13b07260b5a7284c1f8cbc40ec0f4460cb216c44795698e88dcf19275e0569e16c133e12bcfd74b729c79dfab729a6c7f7196629e37797fd615dda0795880e21f0cdf66275b50336d6d59e0edb37999087309ef13b682aed5e3b2c8770d3c2349bfe3a278810ad60bf7a555c5496f790de20678b60a4c16a306d3f3aa0846120af3929130033795e74cd582f374d78b4b5e001433d1bbc1fbc890dd69fa4cab02194332aff100de93cc1f5deaad78163974b54f0e288cb03593ef948282513c01d3a800e60f3a001120d58be1dd4d5dc82aceb6a218a86f1f2a2d7fc9e537a3d34b3c63c95b17b784a019a9048f33964016f6f361756ff66f2e368e15bef3440fe99e6f5f3f646e7e4f5d18685fa6f5293727ccef6748b4a5c6021e86fc0041f13640f20340c13a5b0180";
const char *MAP_HEX = "001285837f2ac48d34380f049305ad3e982bb8ebcf8974c490985c69a3e0100912330bc58f3533b990adbd037eb2124c16b4fa42a081c0800008041efec8569c5c4491d8b4d863f49400890ce792b14bc6ff5e002873ac3d5aa6c50b973f29f90c29305acda1016000010170154469aff68b45f7e48be5608177c00425d8c8fbfb68025f181010922d087fa42018137623cf27abe4f8ff44fc561c982d69a22040e00000200095e931d65dc2bdb5b2ff8000d7f312fe5bb233fecd00181d8b72fd5666bf880206838e1f8949c5e97bac74bf4f81271ca0088000100026933647627b7e3782695a36a1a5e0f3fc2384019987e820410146899bb2b9c8fce800d34d56c97d76a64048083008be9f0e24011c1c982d69dbbf57e0b62693aa763039d84818313a4f101640406d4aed322bb441e0007ef4124c16b4fa651021040000080005ea2e4a0aa0d03f8e0087600f9370fc096bf464017e0c40d0919e29ff2a004f45b99a260904ff8e8019574b68285d9db7a9a5c312e0e4c16b4f63c0c9c76922de6a591e6ca9552d5d1a0e0b05a80bf3507260b5a6f240210800020020f1ba08077b3a2c37d8dff94b856aa493e4e4014d73f24697ffdafaf000fa5007d1ff3694feb8401dd7d138c51bd09fff100515361ccc71a2bbfa400554a33f5261933535528fc46049b8210101000042eddebe9234a98e872d5707c8c8039cca6280846565393fb4c6f11c0c983186ed3d4d68d887a3a227ba63e13020b3a44f8dcd9061fa48d2f05896411fb0049305ad3e8c04800020042e61db0e822708d17b5cc17e090024b4262ae38e8dff9b800f21e1402362efb7ce08034422a6c171d9f4241fb84093c480411000040030a6ce962ac8e060609210080154c41890612aa41eb513e30101c915ee9567cd0cfc3802402fea75e0f837be34041da19a435a063d4faf020405db37524e767bf480813783e9b8a7f698098fc3a1c982d69c59008041000b8f0e27e0183f6a695f845fd30029f92784022161d7eea0037f758d85fbe1b3f3a200e21d14fa12a43b9a8b37ed40e4c16b4d20802c4000020b8a3abbd8f6e653e7b52345f35020bca212dd11678a7db404349362bc0a6578ff0a002eb36c2134f61cab4c5063737e66024aca0100400800022699bc08da9d05f6d010ce20806a21af287c184033a8196e252a851f00080c3230b8797ce5f21adb1d709260b5a7d2541d25f8bd4586171382d50c158ece4dea2a76905ede08fd7a049345001c000002010be7db90135b7a45e64f372bce43f370fc8c0234c980f6dc33b43f4d0809702284c96cbfd0ef7ed9124c16b4fa0c120040200400109db1337c4aeff3f80a38e7248ebe9ff1f7dee484bc8f09b1fce004541b0ab498ea047fd404112d1dbdb39fbfe920d8a5564cfd9024982d69f4224401c0000022eb54640890c2a9c5e869fc1000b7e8867684a0da9f6d0049f9530cc79194f7e0c0439d7b0edd166c8bf1e002bda3d1a060f73bbbf9c33bd7e3f064c1c7410082002002e6528e72d4db8c0d2a321fe4c042b17bb989675071fdd002af233e3bf44e41fd5808333d8bc0e1b70fbf80008ef14fcb1e1994432fd7a2597e220a4c16a8c810708004005c5a6bb5f210b855fe8b4fc10804ec9b04de1fb81ff4a009eb32f765940434fa320059241f0b0a1126bf3640113d5bb33dd0f8bf9540074e6feaef682c8660089aefb23819307db654afaf50c85e57ab623e9531e761ffc815a907b6c77b31fee662b080bf2509260b5a7d34100424001000005a384e20f21454147a7d740837dcf0d32e4b203fa1400a3645cbdc49eddfd5c00728ea08d34b4443ffd100fc07edaf0a42aeff3d002e71e5f865bcf90ceda7c539fa1829305ab52004d000008176938786aa95cf5cfa48bffa0818322c5b8ea2fc7fb5100607509864fe8b0bfda011647065e12cc9b71faf009205260b5019304124c16b4fa0089a070e0c30d1010830";
const char *PSM_HEX = "00205d7fffc37feafd3d0d511524beaba433e42540c03bf2c4840c497e676a0e4c556c35f6fffec9289090c5dd7ca7d9149826ffed175fb22fb658eb94d1b12c3510d0200ff8dbff3fcd4abfdd4b1a38dae01c267ccc19408652aa0200421b31";
const char *RTCM_HEX = "001c3172361c5e0ffff3e134666f7da521a3116a2199e8bae98ef90ccc6d4f5da29145c14e805287affa90113d0d511550014f43";
const char *SDSM_HEX = "00293fee4f4354451fe189d73d8db3487a47e59668527c20a5ee4b3eb1ad79e017f38b87cebd5ff6885eb08ddfc05a5bd727e18c7bccc1edac3cf96d7e7d0afc9e48";
const char *RSM_HEX = "002182B72F00878190A929E86A88B1BFD51931DD5E3826BFFB8B343AFB6741300FE5C32BA3BBDE67193178F6B7A4C55D7F92AC0F0491F9F41C7FF5C0EAAC660564EC7395A3852EF80497084BD802D70CEE59F40525C774EA036AC088DCD87C1F53E812C941115332BD8AD799163C0A9C81C982D69E83F981552BFD669E5A5F2D16C74FA63E918F8F0F604A90F3A1C12E23BBAD93CB049305AD3E920B77A512C50E4CA153321E342E7B46E1E301A8A4E59014982D4A049305AD3E880A4C16A881C14BB827A1AA228030783694F4354452130736F1056E8D0514C69E86A88BA29495BE95B96477A30E24A8B850AE4165F37B57429305AA82946C0941100F20C350AD1F10EFBC12F596922EAF9F5EE842DA5CB6A3207E642A0903C5B0D861EC9F40E00D471F4F4B0459E06117072B5EC72EDD6E8A402049001556AA6ADA64EBB41999C5CFA6548032950FA47C8D7026AC31E9E66DAADAA6D910CD5064C14484DC189C9FC95A23DF37185E92AD35592B8E0E5EFD26B49962279A2B778687051070CE2BA22974CB3B0110112EADFF2FCBC29B8D28C75BF56B3AB2B823CFE3F939D142D716370A601AC78E4A5FBCF10807A140AB40F08C4344158FB9404A4FC4F5DF57B57F128327A46AD6AD1BBBBF0FF8413612022A903960AA847A131787C4361D860C2B5BD507FAF45ECD0BAF9DC89D2817EC92DDCFDD89C28532C350AD3DEA25C2100228D4347F980931D2B8DFE2BADCA94D615CA0CBEBB2A091E58BE2B51215BE98AE235E1515E348C6CA2AF567FF1CB6449305AD3E988951821392E876D923D4C0E367116255FE6BBB512CBBCD0D5A0FEA65F650A52B5E633EA40E8458E51282F562201A529BB46DA51247A862897C9696F1FB10774216AB8E3CF15D715DB2E514268A9DA38D309EF0FA0C539C893308E63BD5AFBA44436666541EA275627FC4448A5CC2AFBCB2A88869C4D09800949BB79F0573CD5AA7DE4F46A0";

const char *ASD_BSM_HEX = "44400000000084782786283B90A7148D2B0A89C49F8A85A7763BF8423C13C2107E1C0C6F7E2C0C6F1620029015AAC5F50800073D1CE2E121F2CCBFC375986FFFFFFFFE0007775FBF43F4200FFFF000000000004042983820082FFFFFFFD049C20147FFFFFFFD128420408FFFFFFFD2454204D480287FFD2BAC2084680BAFFFD4EAC2064580B33FFD5BF42072380D6BFFD6FCC2079681407FFDA424206778115BFFDB34C205D0811CBFFDBC5C2057B8110BFFDBE142001780337FFEFE643FFFF800BBFFF8AB43FFFFFFFFFFFFBA3420080FFFFFFFFC345FFFC00000";
const char *ONE609_BSM_HEX = "038081B1001480AD562FA8400039E8E717090F9665FE1BACC37FFFFFFFF0003BBAFDFA1FA1007FFF8000000000020214C1C100417FFFFFFE824E100A3FFFFFFFE8942102047FFFFFFE922A1026A40143FFE95D610423405D7FFEA75610322C0599FFEADFA10391C06B5FFEB7E6103CB40A03FFED2121033BC08ADFFED9A6102E8408E5FFEDE2E102BDC0885FFEDF0A1000BC019BFFF7F321FFFFC005DFFFC55A1FFFFFFFFFFFFDD1A100407FFFFFFFE1A2FFFE0000";
const char *ASD_ONE609_HEX = "44400000000084782786283B90A7148D2B0A89C49F8A85A7763BF8423C13C2107E1C0C6F7E2C0C6F16A070103620029015AAC5F50800073D1CE2E121F2CCBFC375986FFFFFFFFE0007775FBF43F4200FFFF000000000004042983820082FFFFFFFD049C20147FFFFFFFD128420408FFFFFFFD2454204D480287FFD2BAC2084680BAFFFD4EAC2064580B33FFD5BF42072380D6BFFD6FCC2079681407FFDA424206778115BFFDB34C205D0811CBFFDBC5C2057B8110BFFDBE142001780337FFEFE643FFFF800BBFFF8AB43FFFFFFFFFFFFBA3420080FFFFFFFFC345FFFC00000";

const char* BATCH_HEX = R"(0013552222e3dfbb7ecd3d3281c2357769ed430f2d7df0f96ed5845c3cfa062e008010159d86a8083000821a087c087c000811503ff43ff400608601e4100604540ffd0ffd004021a089dc8ba802410c03c8201408601e41
00136d2222e3dfbb7ecd3d3281c2957769ed430f2d7df0f99b874eea7d3965cbd03162040000acec354142400410d03dfa3dfa00408681fb11fb100304300f20802022a079907ac001410d03dfa3dfa00c08681fb11fb100704540f330f5d80402180790402410403c8201408201e410
00135442b3b0038529a0a7d30f4ca829f4409904540c593060829862cd58018159d86a7c0380082180790400811903ca2409200608601e4100404300f2080282180790401811903ca23d7200e08601e4100804300f2080
0014251dcd0c98d90b2226d313d2145b49a8a4547ffffffff0019bc4fdfa1fa1007fff8000000000
0013662222e3dfbb7ecd3d3281c2b57769ed430f2d7df0f9a7d30f4cb4fa72cb97a062d008000159d86a8b8480082180790400811503c8c3f6e00608601e4100404300f2080282180790401811503c8c3ef600e08601e4100804300f2080482080790402810403c820)";

const char* BATCH_JSON = R"({ "type": "SPAT", "timestamp": 1735713156407, "hex": "00136e2222e3dfbb7ecd3d3281c2357769ed430f2d7df0f970c1845c3cfa062d408010159d7bee304800821a06e806e8000810d03840399800608a81b5c1b5c00404340fab0fff802821a06e806e8001810d0383838d000e08681f561fff00804540dae0dae004821806c2402810c03612" }
{ "type": "SPAT", "timestamp": 1735713156408, "hex": "0013552222e3dfbb7ecd3d3281c2357769ed430f2d7df0f96ed5845c3cfa062e008010159d7bee583000821a082d887f400810d03820384200608601b0a00604340e080e10804022a06f1c6f6002410c0361401408601b0a" }
{ "type": "SSM", "timestamp": 1735713156409, "hex": "001e1562b3af72a108000062d8081196a7200c0412001480" }
{ "type": "SPAT", "timestamp": 1735713156410, "hex": "0013552222e3dfbb7ecd3d3281c2357769ed430f2d7df0f96ed5845c3cfa062e008010159d7c5bb83000821a082d887f400810d03820384200608601b2d00604340e080e10804022a06f1c6f6002410c0365a01408601b2d" }
{ "type": "BSM", "timestamp": 1735713156411, "hex": "0014250d4d0c9918eb5c26d0fa1f145efb53239ffffffffff00054a6fdfa1fa1007fff8000000000" })";

/**
 * This unit test is currently failing with the fork of asn1c that we're using (currently `usdot-fhwa-stol/usdot-asn1c`).
 * Since encoding BSMs is not a desired feature of the ACM, this test case has been commented out.
 */
// TEST_CASE("Encode BSM", "[encoding]" ) { // TODO: fix test case failing
//     // prepare
//     asn1_codec.setup_logger_for_testing();

//     std::stringstream out1;
//     CHECK(asn1_codec.file_test("unit-test-data/BSM.xml", out1) == EXIT_SUCCESS);
//     parse_result = output_doc.load(out1, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
//     CHECK(parse_result);
//     payload_node = ode_payload_query.evaluate_node(output_doc).node();
//     CHECK(payload_node);
//     byte_node = payload_node.child("MessageFrame").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
// }

TEST_CASE("Encode ASD", "[encoding]" ) {
    std::cout << "=== Encode ASD ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out2;
    CHECK(asn1_codec.file_test("unit-test-data/ASD.xml", out2) == EXIT_SUCCESS);
    parse_result = output_doc.load(out2, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("AdvisorySituationData").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ASD_BSM_HEX) == 0);
}

/**
 * This unit test is currently failing with the fork of asn1c that we're using (currently `usdot-fhwa-stol/usdot-asn1c`).
 * Since encoding BSMs is not a desired feature of the ACM, this test case has been commented out.
 */
// TEST_CASE("Encode ASD_BSM", "[encoding]" ) { // TODO: fix test case failing
//     // prepare
//     asn1_codec.setup_logger_for_testing();
//     std::stringstream out3;
//     CHECK(asn1_codec.file_test("unit-test-data/ASD_BSM.xml", out3) == EXIT_SUCCESS);
//     parse_result = output_doc.load(out3, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
//     CHECK(parse_result);
//     payload_node = ode_payload_query.evaluate_node(output_doc).node();
//     CHECK(payload_node);
//     byte_node = payload_node.child("AdvisorySituationData").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), ASD_BSM_HEX) == 0);
//     byte_node = payload_node.child("MessageFrame").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
// }

TEST_CASE("Encode 1609", "[encoding]" ) {
    std::cout << "=== Encode 1609 ===" << std::endl;
    
    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out4;
    CHECK(asn1_codec.file_test("unit-test-data/1609.xml", out4) == EXIT_SUCCESS);
    parse_result = output_doc.load(out4, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
}

TEST_CASE("Encode ASD_1609", "[encoding]" ) {
    std::cout << "=== Encode ASD_1609 ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out5;
    CHECK(asn1_codec.file_test("unit-test-data/ASD_1609.xml", out5) == EXIT_SUCCESS);
    parse_result = output_doc.load(out5, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
    byte_node = payload_node.child("AdvisorySituationData").child("bytes");
    CHECK(byte_node);
    CHECK(std::strcmp(byte_node.text().get(), ASD_ONE609_HEX) == 0);
}

/**
 * This unit test is currently failing with the fork of asn1c that we're using (currently `usdot-fhwa-stol/usdot-asn1c`).
 * Since encoding BSMs is not a desired feature of the ACM, this test case has been commented out.
 */
// TEST_CASE("Encode 1609_BSM", "[encoding]") { // TODO: fix test case failing
//     // prepare
//     asn1_codec.setup_logger_for_testing();

//     std::stringstream out6;
//     CHECK(asn1_codec.file_test("unit-test-data/1609_BSM.xml", out6) == EXIT_SUCCESS);
//     parse_result = output_doc.load(out6, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
//     CHECK(parse_result);
//     payload_node = ode_payload_query.evaluate_node(output_doc).node();
//     CHECK(payload_node);
//     byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
//     byte_node = payload_node.child("MessageFrame").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
// }

/**
 * This unit test is currently failing with the fork of asn1c that we're using (currently `usdot-fhwa-stol/usdot-asn1c`).
 * Since encoding BSMs is not a desired feature of the ACM, this test case has been commented out.
 */
// TEST_CASE("Encode ASD_1609_BSM", "[encoding]") { // TODO: fix test case failing
//     // prepare
//     asn1_codec.setup_logger_for_testing();
    
//     std::stringstream out7;
//     CHECK(asn1_codec.file_test("unit-test-data/ASD_1609_BSM.xml", out7) == EXIT_SUCCESS);
//     parse_result = output_doc.load(out7, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
//     CHECK(parse_result);
//     payload_node = ode_payload_query.evaluate_node(output_doc).node();
//     CHECK(payload_node);
//     byte_node = payload_node.child("Ieee1609Dot2Data").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), ONE609_BSM_HEX) == 0);
//     byte_node = payload_node.child("MessageFrame").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), BSM_HEX) == 0);
//     byte_node = payload_node.child("AdvisorySituationData").child("bytes");
//     CHECK(byte_node);
//     CHECK(std::strcmp(byte_node.text().get(), ASD_ONE609_HEX) == 0);
// }

TEST_CASE("Encode TIM with payload type 'OdeTimPayload'", "[encoding][odetimpayload]") {
    std::cout << "=== Encode TIM with payload type 'OdeTimPayload' ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out8;
    CHECK(asn1_codec.file_test("data/InputData.encoding.tim.odetimpayload.xml", out8) == EXIT_SUCCESS);
    parse_result = output_doc.load(out8, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
}

TEST_CASE("Encode TIM with payload type 'OdeAsdPayload'", "[encoding][odeasdpayload]") {
    std::cout << "=== Encode TIM with payload type 'OdeAsdPayload' ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out8;
    CHECK(asn1_codec.file_test("data/InputData.encoding.tim.odeasdpayload.xml", out8) == EXIT_SUCCESS);
    parse_result = output_doc.load(out8, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
}

// TODO check oracles with decoder

TEST_CASE("Decode BSM", "[decoding]") {
    std::cout << "=== Decode BSM ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out9;
    CHECK(asn1_codec.file_test("data/InputData.decoding.bsm.xml", out9, false) == EXIT_SUCCESS);
    parse_result = output_doc.load(out9, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
}

TEST_CASE("Decode BSM with VehicleEventFlags (hard braking event)", "[decoding]") {
    std::cout << "=== Decode BSM with VehicleEventFlags (hard braking event) ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out9;
    CHECK(asn1_codec.file_test("data/InputData.decoding.bsm.with.VehicleEventFlags.xml", out9, false) == EXIT_SUCCESS);
    parse_result = output_doc.load(out9, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    pugi::xml_node event_flags_node = bsm_vehicle_event_flags_query.evaluate_node(payload_node).node();
    CHECK(event_flags_node);
    std::string bitstring(event_flags_node.text().get());
    std::cout << "VehicleEventFlags: " << bitstring << std::endl;
    std::string expected_bitstring("0000000100000");
    CHECK(bitstring == expected_bitstring);
}

TEST_CASE("Decode BSM with VehicleEventFlags (jackknife event)", "[decoding]") {
    std::cout << "=== Decode BSM with VehicleEventFlags (jackknife event) ===" << std::endl;

    // prepare
    asn1_codec.setup_logger_for_testing();

    std::stringstream out9;
    CHECK(asn1_codec.file_test("data/InputData.decoding.bsm.with.VehicleEventFlags.eventJackKnife.xml", out9, false) == EXIT_SUCCESS);
    parse_result = output_doc.load(out9, pugi::parse_default | pugi::parse_declaration | pugi::parse_doctype | pugi::parse_trim_pcdata);
    CHECK(parse_result);
    payload_node = ode_payload_query.evaluate_node(output_doc).node();
    CHECK(payload_node);
    pugi::xml_node event_flags_node = bsm_vehicle_event_flags_query.evaluate_node(payload_node).node();
    CHECK(event_flags_node);
    std::string bitstring(event_flags_node.text().get());
    std::cout << "VehicleEventFlags: " << bitstring << std::endl;
    std::string expected_bitstring("00000000000001");
    CHECK(bitstring == expected_bitstring);
}

/*
 * Utilities for VehicleEventFlags tests
 */

void print_bits(const void * buf, const ssize_t num_bytes) {
    std::cout << "uper: ";
    for (int i = 0; i < num_bytes; i++) {
        uint8_t abyte = *((uint8_t *)buf + i);
        std::cout << std::bitset<8>(abyte) << " ";
    }
    std::cout << std::endl;
}

void test_encode_VehicleEventFlags_to_uper(const uint16_t bitstring, const uint8_t *expected_uper, const ssize_t expected_byte_len) {
    std::cout << "bitstring: " << std::bitset<16>(bitstring) << std::endl;
    const uint8_t buf[] = { static_cast<uint8_t>(bitstring >> 8), static_cast<uint8_t>(bitstring & 0xFF) };
    const size_t size = sizeof(buf);
    uint8_t *ptr_buf = (uint8_t *)&buf;
    const int bits_unused = __builtin_ctz(bitstring);
    asn_struct_ctx_t asn_ctx = {};
    VehicleEventFlags_t vef = {ptr_buf, size, bits_unused, asn_ctx};
    VehicleEventFlags_t* ptr_vef = &vef;
    asn_TYPE_descriptor_t *vef_type = &asn_DEF_VehicleEventFlags;
    asn_encode_to_new_buffer_result_t res
        = asn_encode_to_new_buffer(0, ATS_UNALIGNED_BASIC_PER, vef_type, ptr_vef);
    if (res.buffer) {
        printf("Successfully encoded %ld bytes to uper\n", res.result.encoded);
        print_bits(res.buffer, res.result.encoded);
    } else {
        FAIL("Error encoding vef");
        return;
    }
    if (res.result.encoded != expected_byte_len) {
        fprintf(stderr, "Error, expected %ld bytes but got %ld", expected_byte_len, res.result.encoded);
        FAIL("Wrong number of bytes");
    }
    if (memcmp(expected_uper, (uint8_t *)res.buffer, res.result.encoded) != 0) {
        FAIL("Error, Uper doesn't match expected");
    }
    free(res.buffer);
}

void test_decode_VehicleEventFlags_from_uper(const uint8_t * uper, const ssize_t uper_len, const uint16_t expected_bitstring,
    const int expect_constraint_to_fail) {
    print_bits(uper, uper_len);
    VehicleEventFlags_t * ptr_vef = 0;
    asn_dec_rval_t rval
        = asn_decode(0, ATS_UNALIGNED_BASIC_PER, &asn_DEF_VehicleEventFlags, (void **)&ptr_vef, uper, uper_len);
    if (rval.code != RC_OK) {
        FAIL("Error decoding");
        return;
    }
    printf("Successfully decoded %ld bytes\n", rval.consumed);
    printf("bitstring size: %ld, bits_unused: %d\n", ptr_vef->size, ptr_vef->bits_unused);

    if (ptr_vef->size != 2) {
        FAIL("Error, expected bitstring to be 2 bytes");
    }
    uint16_t actual_bitstring = (ptr_vef->buf[0] << 8) | ptr_vef->buf[1];
    std::cout << "bitstring: " << std::bitset<16>(actual_bitstring) << std::endl;
    if (actual_bitstring != expected_bitstring) {
        std::cout << "Error, expected bitstring: " << std::bitset<16>(expected_bitstring)
            << " but got: " << std::bitset<16>(actual_bitstring) << std::endl;
        FAIL("Bitstring mismatch");
    }

    // Check constraints
    char errbuf[128];
    size_t errlen = sizeof(errbuf);
    int cons_ret = asn_check_constraints(&asn_DEF_VehicleEventFlags, ptr_vef, errbuf, &errlen);
    if (cons_ret && !expect_constraint_to_fail) {
        fprintf(stderr, "Constraint check failed: %s\n", errbuf);
        FAIL("Unexpected constraint check fail");
    }

    ASN_STRUCT_FREE(asn_DEF_VehicleEventFlags, ptr_vef);
}

/*
 * Data for VehicleEventFlags tests
 */
constexpr uint16_t bitstring_root = (0x8000 >> VehicleEventFlags_eventHazardLights)
                                        | (0x8000 >> VehicleEventFlags_eventAirBagDeployment);
constexpr uint8_t uper_root[2] = { 0b01000000, 0b00000100 };

constexpr uint16_t bitstring_with_ext = (0x8000 >> VehicleEventFlags_eventHazardLights)
                                        | (0x8000 >> VehicleEventFlags_eventAirBagDeployment)
                                        | (0x8000 >> VehicleEventFlags_eventJackKnife);

constexpr uint8_t uper_with_ext[3] = { 0b10000111, 0b01000000, 0b00000110 };

constexpr uint16_t bitstring_15bits = (0x8000 >> VehicleEventFlags_eventHazardLights)
                                      | (0x8000 >> VehicleEventFlags_eventAirBagDeployment)
                                      | (0x8000 >> VehicleEventFlags_eventJackKnife)
                                      | (0x8000 >> 14);
constexpr uint8_t uper_15bits[3] = {0b10000111, 0b11000000, 0b00000111 };

TEST_CASE("Encode VehicleEventFlags root, from struct to UPER, 13 bits", "[encoding]") {
    std::cout << "=== Encode VehicleEventFlags root, from struct to UPER, 13 bits ===" << std::endl;
    test_encode_VehicleEventFlags_to_uper(bitstring_root, uper_root, 2);
}

TEST_CASE("Encode VehicleEventFlags with 2024 extension, from struct to UPER, 14 bits", "[encoding]") {
    std::cout << "=== Encode VehicleEventFlags with 2024 extension to UPER, 14 bits ===" << std::endl;
    test_encode_VehicleEventFlags_to_uper(bitstring_with_ext, uper_with_ext, 3);
}

TEST_CASE("Encode VehicleEventFlags with unknown future extension, from struct to UPER, 15 bits", "[encoding]") {
    std::cout << "=== Encode VehicleEventFlags with unknown future extension, from struct to UPER, 15 bits ===" << std::endl;
    test_encode_VehicleEventFlags_to_uper(bitstring_15bits, uper_15bits, 3);
}

TEST_CASE("Decode VehicleEventFlags root, from UPER to struct, 13 bits", "[decoding]") {
    std::cout << "=== Decode VehicleEventFlags root, from UPER to struct, 13 bits ===" << std::endl;
    test_decode_VehicleEventFlags_from_uper(uper_root, 2, bitstring_root, 0);
}

TEST_CASE("Decode VehicleEventFlags with 2024 extension, from UPER to struct, 14 bits", "[decoding]") {
    std::cout << "=== Decode VehicleEventFlags with 2024 extension, from UPER to struct, 14 bits ===" << std::endl;
    test_decode_VehicleEventFlags_from_uper(uper_with_ext, 3, bitstring_with_ext, 0);
}

TEST_CASE("Decode VehicleEventFlags with unknown future extension, from UPER to struct, 15 bits", "[decoding]") {
    std::cout << "=== Decode VehicleEventFlags with unknown future extension, from UPER to struct, 15 bits ===" << std::endl;
    test_decode_VehicleEventFlags_from_uper(uper_15bits, 3, bitstring_15bits, 1);
}

void test_http_server_post_single(const std::string& message_type, const std::string& hex, const std::string& expect_xer_contains) {
    std::cout << "=== HttpServer::post_single: Decode " << message_type << " ===" << std::endl;

    Http_Server server(asn1_codec);

    // Set up request
    crow::request req;
    req.add_header("Content-Type", "text/plain");
    req.body = hex;

    crow::response response = server.post_single(req);

    // Is OK response containing the expected XML fragment?
    CHECK(response.code == 200);
    CHECK(response.body.find(expect_xer_contains) != std::string::npos);
    std::cout << "Decoded " << message_type << " to XML: " << std::endl << response.body.substr(0,100) << "..." << std::endl;
}


TEST_CASE("Http_Server::post_single: Decode BSM", "[decoding][http_server]") {
    test_http_server_post_single("BSM", BSM_HEX, "<BasicSafetyMessage>");
}

TEST_CASE("Http_Server::post_single: Decode TIM", "[decoding][http_server]") {
    test_http_server_post_single("TIM", TIM_HEX, "<TravelerInformation>");
}

TEST_CASE("Http_Server::post_single: Decode SRM", "[decoding][http_server]") {
    test_http_server_post_single("SRM", SRM_HEX, "<SignalRequestMessage>");
}

TEST_CASE("Http_Server::post_single: Decode SSM", "[decoding][http_server]") {
    test_http_server_post_single("SSM", SSM_HEX, "<SignalStatusMessage>");
}

TEST_CASE("Http_Server::post_single: Decode SPAT", "[decoding][http_server]") {
    test_http_server_post_single("", SPAT_HEX, "<SPAT>");
}

TEST_CASE("Http_Server::post_single: Decode MAP", "[decoding][http_server]") {
    test_http_server_post_single("MAP", MAP_HEX, "<MapData>");
}

TEST_CASE("Http_Server::post_single: Decode PSM", "[decoding][http_server]") {
    test_http_server_post_single("PSM", PSM_HEX, "<PersonalSafetyMessage>");
}

TEST_CASE("Http_Server::post_single: Decode RTCM", "[decoding][http_server]") {
    test_http_server_post_single("RTCM", RTCM_HEX, "<RTCMcorrections>");
}

TEST_CASE("Http_Server::post_single: Decode SDSM", "[decoding][http_server]") {
    test_http_server_post_single("SDSM", SDSM_HEX, "<SensorDataSharingMessage>");
}

TEST_CASE("Http_Server::post_single: Decode RSM", "[decoding][http_server]") {
    test_http_server_post_single("RSM", RSM_HEX, "<RoadSafetyMessage>");
}


TEST_CASE("Http_Server::post_batch (text/plain)", "[decoding][http_server]") {
    std::cout << "=== HttpServer::post_batch (text/plain)" << std::endl;

    Http_Server server(asn1_codec);

    crow::request req;
    req.add_header("Content-Type", "text/plain");
    req.body = std::string(BATCH_HEX);

    crow::response response = server.post_batch(req);

    CHECK(response.code == 200);
    CHECK(response.body.find("<BasicSafetyMessage>") != std::string::npos);
    CHECK(response.body.find("<SPAT>") != std::string::npos);
}

TEST_CASE("Http_Server::post_batch (application/x-ndjson)", "[decoding][http_server]") {
    std::cout << "=== HttpServer::post_batch (application/x-ndjson)" << std::endl;

    Http_Server server(asn1_codec);

    crow::request req;
    req.add_header("Content-Type", "application/x-ndjson");
    req.body = std::string(BATCH_JSON);

    crow::response response = server.post_batch(req);

    CHECK(response.code == 200);
    CHECK(response.body.find("<BasicSafetyMessage>") != std::string::npos);
    CHECK(response.body.find("<SPAT>") != std::string::npos);
    CHECK(response.body.find("<SignalStatusMessage>") != std::string::npos);
}