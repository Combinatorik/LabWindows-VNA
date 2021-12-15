<distribution version="17.0.0" name="Frequency Analyser" type="MSI64">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{F85C5256-B384-4548-BE61-C9993AC067A0}" x64="true">
		<general appName="Frequency Analyser" outputLocation="c:\Users\vsergan\Desktop\Test Gen V4\cvidistkit.Frequency Analyser" relOutputLocation="cvidistkit.Frequency Analyser" outputLocationWithVars="c:\Users\vsergan\Desktop\Test Gen V4\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.1">
			<arp company="" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="English" showPaths="true" showRuntimeOnly="true" readMe="" license="">
			<dlgstrings welcomeTitle="Frequency Analyser" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="C:" dirID="100" parentID="-1" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="Frequency Analyser" dirID="101" parentID="100" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="Frequency Analyser" dirID="102" parentID="7" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\Users\vsergan\Desktop\Test Gen V4\cvibuild.Test Gen v4\Release64\Test Gen.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="c:\Users\vsergan\Desktop\Test Gen V4\Multi-Function-Synch AI-AO.uir" relSourcePath="Multi-Function-Synch AI-AO.uir" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput64 targetType="0" dirID="101" projectID="0">
				<fileID>0</fileID></projectOutput64>
			<projectDependencies64 dirID="101" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="Test Gen v4" targetFileID="0" destDirID="102" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine installToAppDir="false" activeXsup="false" analysis="true" cvirte="true" dotnetsup="true" instrsup="true" lowlevelsup="true" lvrt="true" netvarsup="true" rtutilsup="false">
			<hasSoftDeps/>
			<doNotAutoSelect>
			<component>activeXsup</component>
			<component>rtutilsup</component>
			</doNotAutoSelect></runtimeEngine><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WIN7_SP1</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<baselineProducts>
			<product name="NI-DAQmx Runtime 16.0.1" UC="{923C9CD5-A0D8-4147-9A8D-998780E30763}" productID="{59E0381B-3C4F-4CAA-AEBF-0985190748C9}" path="(None)" flavorID="_full_" flavorName="Runtime" verRestr="false" coreVer="16.1.49152">
				<dependencies>
					<productID>{03EC2792-E932-419A-ACCB-B376B835A921}</productID>
					<productID>{047392F7-F447-4947-AD5C-D74E7F0F4AA9}</productID>
					<productID>{0869FCA0-E7F7-45B4-8AA6-C7F38BDC999A}</productID>
					<productID>{09601777-0CAB-490F-AA12-10A51406C2BE}</productID>
					<productID>{0BEC53B4-1F11-4B5D-8823-7CD989A2AF18}</productID>
					<productID>{0CADBF06-C737-4310-AC15-5D59AB34A33C}</productID>
					<productID>{0EF046E4-E973-4851-818F-938552DB2E07}</productID>
					<productID>{1691794E-849D-439E-A0FF-D219981401E4}</productID>
					<productID>{17349EEB-C92B-4E22-9BC0-C6D5CD6E380C}</productID>
					<productID>{18C5DCAB-EF32-48F8-9789-2A33DE1CA2CB}</productID>
					<productID>{1943DAB5-9801-4C30-8003-358ED7DC47DA}</productID>
					<productID>{214BEF10-ED6E-4198-A7EC-893BC3550330}</productID>
					<productID>{24A552EE-82DC-47B0-9525-66D67C3BD5ED}</productID>
					<productID>{26B72B11-2EF9-4F28-9D4B-9ABD3E854149}</productID>
					<productID>{2F96C96B-1F8C-4E43-A68E-097198BB94DB}</productID>
					<productID>{3CBE920F-1BF4-49CE-BC5F-6489930404DB}</productID>
					<productID>{48E3422A-2764-4AC0-9027-D066001158C7}</productID>
					<productID>{4A548403-7448-4B6D-8A8E-F41F53AE4A3F}</productID>
					<productID>{5691A0DE-3987-4595-A7F8-91133A3C1437}</productID>
					<productID>{5D9D3011-1C42-4AC1-BD45-5C6DDCB1F0DF}</productID>
					<productID>{5F956678-03F3-4CF5-88C4-78E8181E2081}</productID>
					<productID>{5FE7B722-6061-4AFA-A26F-2979CB4FEA40}</productID>
					<productID>{69C9F553-27CB-49B8-8081-699321DC08EB}</productID>
					<productID>{6EE383F5-6C4D-497F-8175-75EF7FCCE495}</productID>
					<productID>{6EF29D99-5FE4-4D63-87DE-0644222061A2}</productID>
					<productID>{76808B40-06AB-453D-8DF3-B0997CF6BA2E}</productID>
					<productID>{78C76251-4DA2-4142-9C57-933B559F6E18}</productID>
					<productID>{7B34F0FB-C852-460E-BA4F-305AB0362212}</productID>
					<productID>{7C7042C2-507F-4A22-AE36-422F57AD36B6}</productID>
					<productID>{9BF263B3-9781-4FA3-AAD8-A45056E5E8C2}</productID>
					<productID>{A48A9EA9-23E1-4D60-96D7-C05AB141AE2E}</productID>
					<productID>{A4FAF873-938B-43B8-BBF2-E4B2C0CE730B}</productID>
					<productID>{AD02775B-D07D-45F9-80C1-3FD7079FD63C}</productID>
					<productID>{AD9B4F25-F129-4B51-9C05-42F8BF9A5242}</productID>
					<productID>{B1179EA6-D139-42CF-81BE-CCE21E101A20}</productID>
					<productID>{B217F366-D7C0-4807-9F07-5FBC80113A87}</productID>
					<productID>{BB17C0CA-B765-46EC-BA84-C739D0726D6A}</productID>
					<productID>{C1812216-15C3-4A08-9547-D96340259541}</productID>
					<productID>{DA09F5A1-AA07-467D-A0B5-31D9EC8B5048}</productID>
					<productID>{DB97EB98-C805-4510-862C-FACF7C7F9917}</productID>
					<productID>{DDFE3456-19AC-42FC-92C1-CFC10E43A97D}</productID>
					<productID>{DEC25D81-2317-47F6-8B26-D54A939DA1EE}</productID>
					<productID>{E3548E19-28AB-41D8-8155-D3A578110A81}</productID>
					<productID>{E6E0A909-7F30-4EE8-A493-F0329B57B187}</productID>
					<productID>{EF31E898-627A-4DDE-B748-2014F93F8D30}</productID>
					<productID>{FE107E74-49F0-43F3-A0CA-5FAC52592F98}</productID>
					<productID>{FE7D71A2-BF64-441C-9489-BE6F6937B98E}</productID>
					<productID>{FF56A0C1-3413-4024-89A6-0FB565B2E78C}</productID></dependencies></product>
			<product name="NI LabWindows/CVI Shared Runtime 2017" UC="{80D3D303-75B9-4607-9312-E5FC68E5BFD2}" productID="{28BFBCB6-5EEE-47F1-A894-FCD877057827}" path="(None)" flavorID="_full_" flavorName="Full" verRestr="false" coreVer="17.0.49157">
				<dependencies>
					<productID>{0EF046E4-E973-4851-818F-938552DB2E07}</productID>
					<productID>{13774AE4-124D-48C2-A8AC-F8070918385E}</productID>
					<productID>{17349EEB-C92B-4E22-9BC0-C6D5CD6E380C}</productID>
					<productID>{18C5DCAB-EF32-48F8-9789-2A33DE1CA2CB}</productID>
					<productID>{29FBD1D9-6A19-4CF6-935F-C0EFCFB6CA3D}</productID>
					<productID>{2D894BCB-6069-418C-BF94-8E5915D8867C}</productID>
					<productID>{2FC99AB8-F11D-4938-B14F-AC60D1016FE7}</productID>
					<productID>{35E63DC2-6BCC-48D6-A1C5-5B068DF34117}</productID>
					<productID>{4A548403-7448-4B6D-8A8E-F41F53AE4A3F}</productID>
					<productID>{5691A0DE-3987-4595-A7F8-91133A3C1437}</productID>
					<productID>{5E99102B-D07A-4D85-8D50-4BE55E3633A0}</productID>
					<productID>{5F956678-03F3-4CF5-88C4-78E8181E2081}</productID>
					<productID>{5FE7B722-6061-4AFA-A26F-2979CB4FEA40}</productID>
					<productID>{68428815-FD93-4897-BC4A-EC5E3194C4E7}</productID>
					<productID>{6EF29D99-5FE4-4D63-87DE-0644222061A2}</productID>
					<productID>{A4FAF873-938B-43B8-BBF2-E4B2C0CE730B}</productID>
					<productID>{BEA1952D-1F00-4336-9A76-C41FA181C02D}</productID>
					<productID>{CE7D38A4-66ED-4D33-A385-D4A2CF4BFF81}</productID>
					<productID>{D357A74D-E186-4E4B-AF78-E035D3E83368}</productID>
					<productID>{D4E86102-76C1-458C-B0C2-F42B43CAD3CC}</productID>
					<productID>{DA09F5A1-AA07-467D-A0B5-31D9EC8B5048}</productID>
					<productID>{DBAAC357-96E7-459F-B115-B14E29F652EF}</productID>
					<productID>{E2821E0B-CB08-41BD-AE01-A9068F7B8771}</productID>
					<productID>{F05EC50F-B2B4-4B87-A32C-6F9AD11D1B49}</productID>
					<productID>{FE7D71A2-BF64-441C-9489-BE6F6937B98E}</productID>
					<productID>{FF35E2A2-9214-4B76-91AE-5567076AAE8C}</productID>
					<productID>{FF56A0C1-3413-4024-89A6-0FB565B2E78C}</productID></dependencies></product></baselineProducts>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\Users\vsergan\Desktop\Test Gen V4\Test Gen v4.prj" ProjectRelativePath="Test Gen v4.prj"/></Projects>
		<buildData progressBarRate="0.313318670844610">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.060457000000000</ProductsAdded>
				<DPConfigured>1.247808500000000</DPConfigured>
				<DPMergeModulesAdded>3.675452500000000</DPMergeModulesAdded>
				<DPClosed>11.775451499999999</DPClosed>
				<DistributionsCopied>22.100651999999997</DistributionsCopied>
				<End>319.163871500000027</End></progressTimes></buildData>
	</msi>
</distribution>
