myid = -1;

function set_uid(x)
	myid = x;
end
function event_MonsterC_TypeYes(getid)
	API_MonsterYesHeroFind(myid,getid);
end

function event_MonsterC_TypeNo()
	API_MonsterNoHeroFind(myid);
end
