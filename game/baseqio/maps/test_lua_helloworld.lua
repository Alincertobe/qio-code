game.Print("Hello lua world, from ",_VERSION,"!\n")


function runCrateFrame(self)
	--game.Print("runCreateFrame: self is ",self); -- Dusan: Nobody want and like <3 massive console spam
end

function G_InitGame()
	local i = 0
	local j = 0

	for i=0,10,1 do
	  for j=0,10,1 do
		--game.Print("i is ",i, " and j is ",j); -- Dusan: Nobody want and like <3 massive console spam
		local ent = entity.Spawn("ModelEntity");
		ent:SetRenderModel("models/props/crates/crate1.obj");
		ent:SetOrigin(vector.Construct(i*32, j*32, 16));
		ent:AddEventHandler("runFrame",runCrateFrame);
		--game.Print(ent) -- Dusan: Nobody want and like <3 massive console spam
	  end
	end
end






















