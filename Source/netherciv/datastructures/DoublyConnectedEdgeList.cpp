// Fill out your copyright notice in the Description page of Project Settings.


#include "DoublyConnectedEdgeList.h"
#include "netherciv/util/Util.h"
#include "KDTree.h"
#include "netherciv/bitmap/BMPImage.h"

DoublyConnectedEdgeList::DoublyConnectedEdgeList()
{
	UE_DIST_GLOBE_RADIUS = 1000;
}

DoublyConnectedEdgeList::~DoublyConnectedEdgeList()
{
}

DoublyConnectedEdgeList* DoublyConnectedEdgeList::CreateGoldbergPolyhedronFromSubdividedIcosahedron()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::CreateGoldbergPolyhedronFromSubdividedIcosahedron)

	//Get center of adjacent faces, make new list of vertices for the hex globe
	TArray<vertex*> hexGlobeVertices = GenerateHexGlobeVertices();

	DoublyConnectedEdgeList* hexDcel = new DoublyConnectedEdgeList();
	hexDcel->vertices = hexGlobeVertices;
	hexDcel->originalVertices = originalVertices;

	//calculate half edges
	hexDcel->adjacentVertices = hexDcel->GetVertexAdjacencies(hexDcel->vertices, true);
	hexDcel->halfEdgesBetweenVertices = hexDcel->GetHalfEdgesBetweenVertices(hexDcel->adjacentVertices);

	hexDcel->DoClockwiseAssignment(true);
	hexDcel->GetFacesFromHalfEdges(hexDcel->halfEdgesBetweenVertices);
	UE_LOG(LogTemp, Display, TEXT("hex globe faces total = %d"), hexDcel->faces.Num());

	return hexDcel;
}

void DoublyConnectedEdgeList::CalculateHalfEdges(bool isHexGlobe) {
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::CalculateHalfEdges)

	adjacentVertices = GetVertexAdjacencies(vertices, isHexGlobe);
	halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(adjacentVertices);
}

void DoublyConnectedEdgeList::ReadFromFile(int subd) {
	FString fpath = "C:/temp/subd";
	fpath += FString::FromInt(subd);
	fpath += ".txt";

	FString filePath = FPaths::ConvertRelativePathToFull(fpath);
	FString fileContent = "";
	UE_LOG(LogTemp, Display, TEXT("Loading file"));
	FFileHelper::LoadFileToString(fileContent, *filePath);

	TArray<FString> contentLines;
	fileContent.ParseIntoArrayLines(contentLines);

	vertices = {};
	int lineIndex = 2;	//start at 2 to skip "VERTICES (x, y, z), and count"
	while (contentLines[lineIndex] != FString("HALF_EDGES (index, tail(vertex), twin, prev, next)")) {
		TArray<FString> coordinates;
		contentLines[lineIndex].ParseIntoArray(coordinates, TEXT(","));

		vertex* v = new vertex();
		v->verticesIndex = FCString::Atoi(*(coordinates[0]));
		v->location.X = FCString::Atod(*(coordinates[1]));
		v->location.Y = FCString::Atod(*(coordinates[2]));
		v->location.Z = FCString::Atod(*(coordinates[3]));
		vertices.Add(v);

		lineIndex++;
	}
	lineIndex++;
	
	halfEdgesBetweenVertices = {};
	int numHalfEdges = FCString::Atoi(*(contentLines[lineIndex]));
	lineIndex++;

	TArray<half_edge*> heArray = {};
	for (int i = 0; i < numHalfEdges; i++) {
		heArray.Add(new half_edge());
	}
	
	while (contentLines[lineIndex] != FString("FACES (half_edge_indices)")) {
		TArray<FString> refs;
		contentLines[lineIndex].ParseIntoArray(refs, TEXT(","));

		int selfIndex = FCString::Atoi(*(refs[0]));
		int tailVertexIndex = FCString::Atoi(*(refs[1]));
		int twinIndex = FCString::Atoi(*(refs[2]));
		int prevIndex = FCString::Atoi(*(refs[3]));
		int nextIndex = FCString::Atoi(*(refs[4]));

		half_edge* h = heArray[selfIndex];
		h->tail = vertices[tailVertexIndex];
		h->twin = heArray[twinIndex];
		h->prev = heArray[prevIndex];
		h->next = heArray[nextIndex];

		lineIndex++;
	}
	lineIndex++;
	lineIndex++;

	faces = {};
	while (lineIndex < contentLines.Num()) {
		face* newFace = new face();
		newFace->reps = {};

		TArray<FString> refs;
		contentLines[lineIndex].ParseIntoArray(refs, TEXT(","));
		for (int i = 0; i < refs.Num(); i++) {
			newFace->reps.Add(heArray[FCString::Atoi(*(refs[i]))]);
		}
		faces.Add(newFace);

		lineIndex++;
	}


	UE_LOG(LogTemp, Display, TEXT("Done loading file"));
}

void DoublyConnectedEdgeList::WriteToFile(int subd)
{
	FString serializedString = "";
	serializedString += "VERTICES (x, y, z)\n";
	serializedString += FString::FromInt(vertices.Num());
	serializedString += "\n";
	for (vertex* v : vertices) {
		serializedString += FString::FromInt(v->verticesIndex);
		serializedString += ",";
		serializedString += FString::SanitizeFloat(v->location.X);
		serializedString += ",";
		serializedString += FString::SanitizeFloat(v->location.Y);
		serializedString += ",";
		serializedString += FString::SanitizeFloat(v->location.Z);
		serializedString += "\n";
	}


	//get half-edges into array, map
	TArray<half_edge*> h = {};
	TMap<half_edge*, int> h_indices = {};

	TArray<TMap<vertex*, half_edge*>> halfEdgesValues = {};
	halfEdgesBetweenVertices.GenerateValueArray(halfEdgesValues);
	for (int i = 0; i < halfEdgesValues.Num(); i++) {
		TArray<half_edge*> halfEdgesFromMap = {};
		halfEdgesValues[i].GenerateValueArray(halfEdgesFromMap);
		for (int j = 0; j < halfEdgesFromMap.Num(); j++) {
			h_indices.Add(halfEdgesFromMap[j], h.Num());
			h.Add(halfEdgesFromMap[j]);
		}
	}

	serializedString += "HALF_EDGES (index, tail(vertex), twin, prev, next)\n";
	serializedString += FString::FromInt(h.Num());
	serializedString += "\n";
	for (int i = 0; i < h.Num(); i++) {
		serializedString += FString::FromInt(h_indices[h[i]]);
		serializedString += ",";
		serializedString += FString::FromInt(h[i]->tail->verticesIndex);
		serializedString += ",";
		serializedString += FString::FromInt(h_indices[h[i]->twin]);
		serializedString += ",";
		serializedString += FString::FromInt(h_indices[h[i]->prev]);
		serializedString += ",";
		serializedString += FString::FromInt(h_indices[h[i]->next]);
		serializedString += "\n";
	}

	serializedString += "FACES (half_edge_indices)\n";
	serializedString += FString::FromInt(faces.Num());
	for (int i = 0; i < faces.Num(); i++) {
		serializedString += "\n";
		for (int j = 0; j < faces[i]->reps.Num(); j++) {
			serializedString += FString::FromInt(h_indices[faces[i]->reps[j]]);
			
			if (j != faces[i]->reps.Num() - 1) {
				serializedString += ",";
			}
		}
	}

	FString fpath = "C:/temp/subd";
	fpath += FString::FromInt(subd);
	fpath += ".txt";

	FString FilePath = FPaths::ConvertRelativePathToFull(fpath);
	FFileHelper::SaveStringToFile(serializedString, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}

bool DoublyConnectedEdgeList::IsTriangle(face* face_in) {
	return face_in->reps.Num() == 3;
}
bool DoublyConnectedEdgeList::IsPentagon(face* face_in) {
	return face_in->reps.Num() == 5;
}
bool DoublyConnectedEdgeList::IsHexagon(face* face_in) {
	return face_in->reps.Num() == 6;
}

//UV0s not working
void DoublyConnectedEdgeList::PrepareVerticeLocationsAndTriangles()
{

	//save bitmap
	//int bmpWidth = 640;
	//int bmpHeight = 480;

	//BMPImage bmpImage(bmpWidth, bmpHeight);

	//for (int y = 0; y < bmpHeight; y++) {
	//	for (int x = 0; x < bmpWidth; x++) {
	//		bmpImage.SetColor(Color((float)x / (float)bmpWidth, 1.0f - ((float)x / (float)bmpWidth), (float)y / (float)bmpHeight), x, y);
	//	}
	//}
	//UE_LOG(LogTemp, Display, TEXT("Saving bitmap"));
	////bmpImage.Export("C:/temp/subd/myOutputImage.bmp");
	//UE_LOG(LogTemp, Display, TEXT("Done saving file"));

	
	//load bitmap
	BMPImage globeImage(0, 0);	//you can make a default constructor...
	globeImage.Read("C:/temp/subd/equirectangularProjection_cropped.bmp");
	//copy.Export("C:/temp/subd/equirectangularProjectionButCopied.bmp");



	//original implementation
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::PrepareVerticeLocationsAndTrianglesAndUV0s)

	verticeLocations = {};
	triangles = {};

	UE_LOG(LogTemp, Display, TEXT("Total faces: %d"), faces.Num());

	for (face* faceRef : faces) {
		TArray<FVector> faceVertices = {};

		FVector midpoint = FVector(0, 0, 0);
		for (int i = 0; i < faceRef->reps.Num(); i++) {
			faceVertices.Add(faceRef->reps[i]->tail->location);

			midpoint += faceRef->reps[i]->tail->location;
		}
		midpoint /= faceRef->reps.Num();

		int faceVerticesNum = faceVertices.Num();
		faceVertices.Add(midpoint);

		TArray<int> faceTriangles = {};
		for (int i = 0; i < faceVerticesNum; i++) {
			faceTriangles.Add(i);
			faceTriangles.Add((i + 1) % faceVerticesNum);
			faceTriangles.Add(faceVerticesNum);	//middle of face
		}


		TArray<FVector2D> faceUv0s = {};
		for (int i = 0; i < faceVertices.Num(); i++) {

			//original thing
			bool water = rand() % 2 == 0;
			if (water) {
				faceUv0s.Add(FVector2D(0.01 * (i + 1), 0.01 * (i + 1)));
			}
			else {
				faceUv0s.Add(FVector2D((0.01 * (i + 1)) + 0.6, (0.01 * (i + 1)) + 0.6));
			}
			
		}

		verticeLocations.Add(faceVertices);
		triangles.Add(faceTriangles);

		allVerticeLocations.Append(faceVertices);
		allTriangles.Append(faceTriangles);
	}



	allVerticeLocations = {};
	allTriangles = {};

	allVerticeLocations = {};
	for (int i = 0; i < vertices.Num(); i++) {
		vertices[i]->verticesIndex = i;
		allVerticeLocations.Add(vertices[i]->location);
	}

	allTriangles = {};
	trianglesBy3s = {};
	for (int f = 0; f < faces.Num(); f++) {
		face* faceRef = faces[f];
		if (IsTriangle(faceRef)) {
			allTriangles.Append({
				faceRef->reps[0]->tail->verticesIndex,
				faceRef->reps[1]->tail->verticesIndex,
				faceRef->reps[2]->tail->verticesIndex
				});
		}
		else {
			FVector midpoint = FVector(0, 0, 0);
			for (int i = 0; i < faceRef->reps.Num(); i++) {
				midpoint += faceRef->reps[i]->tail->location;
			}
			midpoint /= faceRef->reps.Num();
			allVerticeLocations.Add(midpoint);


			//Start new thing
			//new thing
			float atanX = atan(midpoint.Y / midpoint.X);
			float radiansAroundGlobe;
			UE_LOG(LogTemp, Display, TEXT("x:%f, y:%f, atan:%f"), midpoint.X, midpoint.Y, FMath::RadiansToDegrees(atanX));
			if (midpoint.Y > 0 && midpoint.X > 0) {
				radiansAroundGlobe = atanX;
			}
			else if (midpoint.Y > 0 && midpoint.X < 0) {
				radiansAroundGlobe = UE_PI + atanX;
			}
			else if (midpoint.Y < 0 && midpoint.X < 0) {
				radiansAroundGlobe = UE_PI + atanX;
			}
			else if (midpoint.Y < 0 && midpoint.X > 0) {
				radiansAroundGlobe = (2*UE_PI) + atanX;
			}
			else {
				UE_LOG(LogTemp, Display, TEXT("How this happen? x:%f, y:%f"), midpoint.X, midpoint.Y);
			}
			float percentRadiallyAroundGlobe = radiansAroundGlobe / (2 * UE_PI);
			UE_LOG(LogTemp, Display, TEXT("x:%f, y:%f, degreesAroundGlobe:%f, percentRadiallyAroundGlobe:%f"), midpoint.X, midpoint.Y, FMath::RadiansToDegrees(radiansAroundGlobe), percentRadiallyAroundGlobe);
			int pixelX = globeImage.getWidth() * (1.0f-percentRadiallyAroundGlobe);	//gotta invert x-axis

			float percentLinearlyUpGlobe = (midpoint.Z + UE_DIST_GLOBE_RADIUS) / (UE_DIST_GLOBE_RADIUS * 2);	//weird math because the southern hemisphere has negative Z-coord
			int pixelY = globeImage.getHeight() * percentLinearlyUpGlobe;

			UE_LOG(LogTemp, Display, TEXT("Z:%f, percZ:%f"), midpoint.Z, percentLinearlyUpGlobe);

			Color mapPointHexColor = globeImage.GetColor(pixelX, pixelY);
			bool water = mapPointHexColor.r > 0.95;
			//End new thing

			//bool water = rand() % 2 == 0;

			for (int i = 0; i < faceRef->reps.Num(); i++) {
				half_edge* edge = faceRef->reps[i];

				allTriangles.Append({
					edge->tail->verticesIndex,
					edge->next->tail->verticesIndex,
				allVerticeLocations.Num() - 1
					});

				trianglesBy3s.Add(FIntVector(
					edge->tail->verticesIndex,
					edge->next->tail->verticesIndex,
					allVerticeLocations.Num() - 1
				));

				if (water) {
					waterTrianglesBy3s.Add(FIntVector(
						edge->tail->verticesIndex,
						edge->next->tail->verticesIndex,
						allVerticeLocations.Num() - 1
					));
				}
				else {
					landTrianglesBy3s.Add(FIntVector(
						edge->tail->verticesIndex,
						edge->next->tail->verticesIndex,
						allVerticeLocations.Num() - 1
					));
				}
			}
		}
	}
}


TMap<vertex*, TMap<vertex*, half_edge*>> DoublyConnectedEdgeList::GetHalfEdgesBetweenVertices(TMap<vertex*, TArray<vertex*>> adjacentVertices_param) {
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::GetHalfEdgesBetweenVertices)


	TArray<vertex*> adjacentVertices_vertices = {};
	adjacentVertices_param.GetKeys(adjacentVertices_vertices);

	TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices_var = {};
	for (int i = 0; i < adjacentVertices_vertices.Num(); i++) {
		vertex* v1 = adjacentVertices_vertices[i];
		if (!halfEdgesBetweenVertices_var.Contains(v1)) {
			halfEdgesBetweenVertices_var.Add(v1, {});
		}

		for (int j = 0; j < adjacentVertices_param[v1].Num(); j++) {
			vertex* v2 = adjacentVertices_param[v1][j];

			if (!halfEdgesBetweenVertices_var[v1].Contains(v2)) {
				if (!halfEdgesBetweenVertices_var.Contains(v2)) {
					halfEdgesBetweenVertices_var.Add(v2, {});
				}

				TArray<half_edge*> newHalfEdges = DoublyConnectedEdgeList::CreateHalfEdges(v1, v2);

				halfEdgesBetweenVertices_var[v1].Add(v2, newHalfEdges[0]);
				halfEdgesBetweenVertices_var[v2].Add(v1, newHalfEdges[1]);
			}
		}
	}
	return halfEdgesBetweenVertices_var;
}




TMap<vertex*, TArray<vertex*>> DoublyConnectedEdgeList::GetVertexAdjacencies(TArray<vertex*> vertices_param, bool isHexGlobe) {
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::GetVertexAdjacencies)

	using tree_t = jk::tree::KDTree<vertex*, 3>;
	using point_t = std::array<double, 3>;

	tree_t tree;
	for (vertex* v : vertices_param) {
		tree.addPoint(point_t{ {v->location.X, v->location.Y, v->location.Z} }, v);
	}

	adjacentVertices = {};
	double radiusToCheck = 2048.0;
	for (vertex* v : vertices_param) {
		int adjacentVerticeCount;
		if (originalVertices.Contains(v)) {
			adjacentVerticeCount = 5;
		}
		else {
			adjacentVerticeCount = 6;
		}
		if (isHexGlobe) {
			adjacentVerticeCount = 3;
		}

		point_t vPoint = point_t{ v->location.X, v->location.Y, v->location.Z };

		auto potentialAdjacents = tree.searchBall(vPoint, radiusToCheck * radiusToCheck);
		while (potentialAdjacents.size() > adjacentVerticeCount + 1) {
			radiusToCheck /= 2;
			potentialAdjacents = tree.searchBall(vPoint, radiusToCheck * radiusToCheck);
		}
		radiusToCheck *= 2;
		potentialAdjacents = tree.searchBall(vPoint, radiusToCheck * radiusToCheck);
		UE_LOG(LogTemp, Display, TEXT("radius = %f, potentialAdjacents.size= %d"), radiusToCheck, potentialAdjacents.size());

		//+1 because includes self
		auto adjacentVerticesFromKDTree = tree.searchCapacityLimitedBall(vPoint, radiusToCheck * radiusToCheck, adjacentVerticeCount+1);

		TArray<vertex*> verticesAdjacentToV = {};
		for (const auto& adjacentVertex : adjacentVerticesFromKDTree) {
			if (v != adjacentVertex.payload) {	//exclude self
				verticesAdjacentToV.Add(adjacentVertex.payload);
			}
		}

		adjacentVertices.Add(v, verticesAdjacentToV);
	}
	return adjacentVertices;
}


TArray<half_edge*> DoublyConnectedEdgeList::CreateHalfEdges(vertex* vert1, vertex* vert2) {
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::CreateHalfEdges)


	half_edge* e1 = new half_edge();
	half_edge* e2 = new half_edge();

	//Pick one of the half - edges and assign it as the representative for the endpoint.
	vert1->rep = e1;
	vert2->rep = e2;

	e1->tail = vert1;
	e2->tail = vert2;
	e1->twin = e2;
	e2->twin = e1;

	e1->name = e1->twin->tail->name;
	e2->name = e2->twin->tail->name;

	UE_LOG(LogTemp, Display, TEXT("creating half_edge with name %s"), *(e1->name));
	UE_LOG(LogTemp, Display, TEXT("creating half_edge with name %s"), *(e2->name));

	return { e1, e2 };
}

void DoublyConnectedEdgeList::GetFacesFromHalfEdges(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices_param) {
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::GetFacesFromHalfEdges)

	TArray<half_edge*> allHalfEdges = {};

	//populateAll HalfEdges, effectively unpacking the nested map
	TArray<vertex*> outerKeys = {};
	halfEdgesBetweenVertices_param.GetKeys(outerKeys);
	for (int i = 0; i < outerKeys.Num(); i++) {
		vertex* outerKey = outerKeys[i];
		TArray<half_edge*> halfEdges = {};
		halfEdgesBetweenVertices_param[outerKey].GenerateValueArray(halfEdges);
		allHalfEdges.Append(halfEdges);
	}

	faces = {};
	TSet<half_edge*> visitedEdges = {};

	for (int i = 0; i < allHalfEdges.Num(); i++) {
		if (!visitedEdges.Contains(allHalfEdges[i])) {
			half_edge* current_edge = allHalfEdges[i];

			face* newFace = new face();
			newFace->reps = {};
			newFace->name = "";

			while (!visitedEdges.Contains(current_edge)) {
				newFace->name += current_edge->name;

				newFace->reps.Add(current_edge);
				current_edge->left = newFace;
				visitedEdges.Add(current_edge);

				current_edge = current_edge->next;
			}

			faces.Add(newFace);
		}
	}
}

void DoublyConnectedEdgeList::DoClockwiseAssignment(bool isHexGlobe) {
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::DoClockwiseAssignment)

	//3) For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.
	//4) For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
	//5) Pick one of the half-edges and assign it as the representative for the endpoint. (Degenerate case: if there's only one half-edge e in the sorted list, set e->twin->next = e and e->prev = e->twin). The next pointers are a permutation on half-edges.

	TArray<vertex*> half_edges_by_v1 = {};
	halfEdgesBetweenVertices.GetKeys(half_edges_by_v1);

	for (int i = 0; i < half_edges_by_v1.Num(); i++) {
		vertex* v1 = half_edges_by_v1[i];
		//UE_LOG(LogTemp, Display, TEXT("Clockwise for %s:"), *(v1->name));

		int adjacentVerticeCount;
		if (originalVertices.Contains(v1)) {
			adjacentVerticeCount = 5;
		}
		else {
			adjacentVerticeCount = 6;
		}
		if (isHexGlobe) {
			adjacentVerticeCount = 3;
		}

		TArray<half_edge*> halfEdges = {};
		halfEdgesBetweenVertices[v1].GenerateValueArray(halfEdges);

		TArray<half_edge*> clockwiseEdges = {};

		TMap<FVector, half_edge*> half_edges_byTwinTailVector = {};
		//make half edges. Give them tails in clockwise order...
		for (int j = 0; j < adjacentVerticeCount; j++) {
			half_edge* he = halfEdges[j];
			FVector heNormalized = FVector(he->twin->tail->location);
			heNormalized.Normalize();
			half_edges_byTwinTailVector.Add(heNormalized, he);
		}

		half_edge* first = halfEdges[0];
		FVector firstNormalized = FVector(first->twin->tail->location);

		clockwiseEdges.Add(first);
		//UE_LOG(LogTemp, Display, TEXT("first is %s"), *(first->name));

		//get the other side vectors of the half edges
		FVector vNormalized = FVector(v1->location);
		vNormalized.Normalize();



		TSet<FVector> alreadyInTheClockwiseOrder = TSet<FVector>();
		//get angles in clockwise order
		for (int j = 1; j < adjacentVerticeCount; j++) {
			TArray<FVector> halfEdgeVectors = {};
			half_edges_byTwinTailVector.GetKeys(halfEdgeVectors);

			int degreesToRotate = j * 360 / adjacentVerticeCount;
			//UE_LOG(LogTemp, Display, TEXT("degrees to rotate: %d"), degreesToRotate);
			FVector expectedNextVector = Util::RotateRelativeToVectorAndQuat(firstNormalized, vNormalized, FQuat(vNormalized, FMath::DegreesToRadians(degreesToRotate)));
			expectedNextVector.Normalize();
			//UE_LOG(LogTemp, Display, TEXT("expected rotation: "));
			Util::LogVector(expectedNextVector);

			//after this for loop, closestVector is the next clockwise vector
			double closestSoFar = 9999;	//just bigger than 360 i guess
			FVector closestVector = FVector(0, 0, 0);	//it should never remain this, as we already added it above and are now rotating it
			
			for (int k = 0; k < halfEdgeVectors.Num(); k++) {
				if (!alreadyInTheClockwiseOrder.Contains(halfEdgeVectors[k])) {
					FVector toMeasure = halfEdgeVectors[k];
					double offBy = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(expectedNextVector, toMeasure)));

					if (offBy < closestSoFar) {
						closestSoFar = offBy;
						closestVector = toMeasure;
					}
				}
			}
			//UE_LOG(LogTemp, Display, TEXT("next clockwise is %s, off by %.6f"), *(half_edges_byTwinTailVector[closestVector]->name), closestSoFar);
			//UE_LOG(LogTemp, Display, TEXT("with vector:"));
			//Util::LogVector(closestVector);

			clockwiseEdges.Add(half_edges_byTwinTailVector[closestVector]);
			alreadyInTheClockwiseOrder.Add(closestVector);
		}


		for (int j = 0; j < clockwiseEdges.Num(); j++) {
			UE_LOG(LogTemp, Display, TEXT("\t%s"), *(clockwiseEdges[j]->name));

			half_edge* e1 = clockwiseEdges[j];
			half_edge* e2 = clockwiseEdges[(j + 1) % clockwiseEdges.Num()];	//math to rotate the array to 0 at end

			e1->twin->next = e2;
			e2->prev = e1->twin;
		}

	}
}

TArray<vertex*> DoublyConnectedEdgeList::GenerateHexGlobeVertices() {
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::GenerateHexGlobeVertices)


	TArray<vertex*> newHexGlobeVertices = {};
	TSet<face*> visitedFaces = {};
	for (int i = 0; i < vertices.Num(); i++) {
		vertex* v1 = vertices[i];
		TArray<face*> adjacentFaces = {};

		TArray<half_edge*> halfEdgesFacingAway = {};
		halfEdgesBetweenVertices[v1].GenerateValueArray(halfEdgesFacingAway);
		for (int j = 0; j < halfEdgesFacingAway.Num(); j++) {
			if (!visitedFaces.Contains(halfEdgesFacingAway[j]->left)) {
				visitedFaces.Add(halfEdgesFacingAway[j]->left);

				adjacentFaces.Add(halfEdgesFacingAway[j]->left);
			}
		}

		for (int j = 0; j < adjacentFaces.Num(); j++) {
			face* adjacentFace = adjacentFaces[j];
			FVector center = FVector(0, 0, 0);
			for (int k = 0; k < adjacentFace->reps.Num(); k++) {
				center = center + adjacentFace->reps[k]->tail->location;
			}
			center /= adjacentFace->reps.Num();

			vertex* hexGlobeVertex = new vertex();
			hexGlobeVertex->location = center;
			hexGlobeVertex->name = adjacentFace->name;

			newHexGlobeVertices.Add(hexGlobeVertex);
		}
	}
	return newHexGlobeVertices;
}

void DoublyConnectedEdgeList::Subdivide() {
	TRACE_CPUPROFILER_EVENT_SCOPE(DoublyConnectedEdgeList::Subdivide)

	int midpointCounter = 0;
	TSet<half_edge*> visited = {};

	TArray<vertex*> v1s = {};
	halfEdgesBetweenVertices.GetKeys(v1s);
	for (int i = 0; i < v1s.Num(); i++) {
		vertex* v1 = v1s[i];

		TArray<vertex*> v2s = {};
		halfEdgesBetweenVertices[v1].GetKeys(v2s);
		for (int j = 0; j < v2s.Num(); j++) {
			vertex* v2 = v2s[j];
			if (!visited.Contains(halfEdgesBetweenVertices[v1][v2])) {
				FVector heMidpoint = (v1->location + v2->location) / 2;
				heMidpoint = Util::GetVectorAtDistance(heMidpoint, v1->location.Length());

				//ASpherePoint* newSpherePoint = GetWorld()->SpawnActor<ASpherePoint>(spherePoint, FTransform(heMidpoint));
				//newSpherePoint->floatingLabel = "m" + FString::FromInt(midpointCounter);
				//newSpherePoint->Initialize();
				

				vertex* newVertex = new vertex();
				newVertex->location = heMidpoint;
				newVertex->name = "m" + FString::FromInt(midpointCounter);
				vertices.Add(newVertex);

				midpointCounter++;

				visited.Add(halfEdgesBetweenVertices[v1][v2]);
				visited.Add(halfEdgesBetweenVertices[v2][v1]);
			}
		}
	}
}

void DoublyConnectedEdgeList::LoadIcosahedronCartesianCoordinates() {
	TArray<TArray<TArray<double>>> icosahedronCartesianCoordinates =
	{
		{
			{0},
			{1.0, -1.0},
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO},
		},
		{
			{1.0, -1.0},
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO},
			{0},
		},
		{
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO},
			{0},
			{1.0, -1.0}
		}
	};



	vertices = {};
	originalVertices = {};

	TArray<TArray<FVector>> v = {};	//vertex locations

	//calculate rotation so we have a north pole vertice
	float northPoleRotation = atan(1.0 / UE_DOUBLE_GOLDEN_RATIO);
	UE_LOG(LogTemp, Display, TEXT("north pole rotation calculated to be %f degrees"), FMath::RadiansToDegrees(northPoleRotation));

	int counter = 0;
	for (int matrix = 0; matrix < 3; matrix++) {
		v.Add({});

		TArray<double> xCoords = icosahedronCartesianCoordinates[matrix][0];
		TArray<double> yCoords = icosahedronCartesianCoordinates[matrix][1];
		TArray<double> zCoords = icosahedronCartesianCoordinates[matrix][2];


		for (int x = 0; x < xCoords.Num(); x++) {
			for (int y = 0; y < yCoords.Num(); y++) {
				for (int z = 0; z < zCoords.Num(); z++) {
					double xCoord = xCoords[x];
					double yCoord = yCoords[y];
					double zCoord = zCoords[z];

					FVector verticeLocation = FVector(xCoord, yCoord, zCoord);
					verticeLocation = Util::RotateRelativeToVectorAndQuat(verticeLocation, FVector(0, 0, 0), FQuat(FVector::YAxisVector, (PI/2)-northPoleRotation));
					verticeLocation = Util::GetVectorAtDistance(verticeLocation, UE_DIST_GLOBE_RADIUS);
					v[matrix].Add(verticeLocation);

					//ASpherePoint* newSpherePoint = GetWorld()->SpawnActor<ASpherePoint>(spherePoint, FTransform(verticeLocation));
					//newSpherePoint->floatingLabel = FString::FromInt(counter);
					

					vertex* newVertex = new vertex();
					newVertex->location = verticeLocation;
					newVertex->name = FString::FromInt(counter);
					counter++;

					vertices.Add(newVertex);
					originalVertices.Add(newVertex);

					//if (matrix == 0) {
					//	newSpherePoint->color = FColor::Red;
					//}
					//if (matrix == 1) {
					//	newSpherePoint->color = FColor::Yellow;
					//}
					//if (matrix == 2) {
					//	newSpherePoint->color = FColor::Blue;
					//}

					//newSpherePoint->Initialize();
				}
			}
		}

	}

	//UE_LOG(LogTemp, Display, TEXT("created %d vertices"), vertices.Num());
}
